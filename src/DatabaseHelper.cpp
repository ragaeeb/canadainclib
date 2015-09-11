#include "DatabaseHelper.h"
#include "IOUtils.h"
#include "Logger.h"
#include "TextUtils.h"

#include <QCoreApplication>
#include <QDir>
#include <QtCore>

#define CHECK_ENABLED if (!m_enabled) return

namespace {

QStringList createDB(QString const& dbPath, QStringList const& setupStatements)
{
    canadainc::IOUtils::writeFile(dbPath);
    return setupStatements;
}

void cleanArguments(QVariantList& args)
{
    for (int i = args.size()-1; i >= 0; i--)
    {
        QVariant c = args[i];
        bool isBool = c.type() == QVariant::Bool;

        if ( !c.isNull() && ( ( c.type() == QVariant::String && c.toString().isEmpty() ) || ( c.type() == QVariant::Double && !c.toDouble() ) || ( c.type() == QVariant::Int && !c.toInt() ) || ( c.type() == QVariant::LongLong && !c.toLongLong() ) || ( isBool && !c.toBool() ) ) )
        {
            c.clear();
            args[i] = c;
        } else if ( isBool && c.toBool() ) {
            c = 1;
            args[i] = c;
        }
    }
}

}

namespace canadainc {

DatabaseHelper::DatabaseHelper(QString const& dbase, QObject* parent) :
        QObject(parent), m_currentId(0), m_enabled(true)
{
    m_sql.setSource(dbase);
    connect( &m_sql, SIGNAL( dataLoaded(int, QVariant const&) ), this, SLOT( dataLoaded(int, QVariant const&) ), Qt::QueuedConnection );
    connect( &m_sql, SIGNAL( error(QString const&) ), this, SIGNAL( error(QString const&) ), Qt::QueuedConnection );
    connect( &m_sql, SIGNAL( setupError(QString const&) ), this, SIGNAL( setupError(QString const&) ), Qt::QueuedConnection );
}


void DatabaseHelper::onDatabaseCreated()
{

}


void DatabaseHelper::attachIfNecessary(QString const& dbase, bool homePath, int id) {
    attachIfNecessary( dbase, homePath ? QDir::homePath() : QString("%1/assets/dbase").arg( QCoreApplication::applicationDirPath() ), id );
}


void DatabaseHelper::attachIfNecessary(QString const& dbase, QString const& path, int id)
{
    CHECK_ENABLED;

    if ( !dbase.isEmpty() && !m_attached.contains(dbase) )
    {
        m_sql.setQuery( QString("ATTACH DATABASE '%1' AS %2").arg( QString("%1/%2.db") ).arg(path).arg(dbase) );
        m_sql.load(id);
        m_attached << dbase;
    }
}


void DatabaseHelper::detach(QString const& dbase, int id)
{
    CHECK_ENABLED;

    if ( m_attached.contains(dbase) )
    {
        m_sql.setQuery( QString("DETACH DATABASE %1").arg(dbase) );
        m_sql.load(id);
        m_attached.remove(dbase);
    }
}


void DatabaseHelper::enableForeignKeys(int id)
{
    CHECK_ENABLED;

    m_sql.setQuery("PRAGMA foreign_keys = ON");
    m_sql.load(id);
}


void DatabaseHelper::dataLoaded(int id, QVariant const& data)
{
    if ( m_idToObjectQueryType.contains(id) )
    {
        QPair<QObject*, int> value = m_idToObjectQueryType[id];
        QObject* caller = value.first;
        int t = value.second;

        m_idToObjectQueryType.remove(id);

        QSet<int> idsForObject = m_objectToIds[caller];
        idsForObject.remove(id);

        if ( !idsForObject.isEmpty() ) {
            m_objectToIds[caller] = idsForObject;
        } else {
            m_objectToIds.remove(caller);
        }

        QMetaObject::invokeMethod(caller, "onDataLoaded", Qt::QueuedConnection, Q_ARG(QVariant, t), Q_ARG(QVariant, data) );
        emit finished(t);
    }
}


void DatabaseHelper::stash(QObject* caller, int t)
{
    ++m_currentId;

    QPair<QObject*, int> pair = qMakePair<QObject*, int>(caller, t);
    m_idToObjectQueryType.insert(m_currentId, pair);

    if ( !m_objectToIds.contains(caller) ) {
        connect( caller, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );
    }

    QSet<int> idsForObject = m_objectToIds[caller];
    idsForObject << m_currentId;
    m_objectToIds[caller] = idsForObject;
}


void DatabaseHelper::executeQuery(QObject* caller, QString const& query, int t, QVariantList const& args)
{
    if (caller) {
        stash(caller, t);
    }

    executeInternal(query, m_currentId, args);
}


void DatabaseHelper::executeInternal(QString const& query, int t, QVariantList args)
{
    CHECK_ENABLED;

    m_sql.setQuery(query);

    if ( args.isEmpty() ) {
        m_sql.load(t);
    } else {
        cleanArguments(args);
        m_sql.executePrepared(args, t);
    }
}


qint64 DatabaseHelper::executeInsert(QString const& table, QVariantMap const& keyValues)
{
    qint64 id = 0;
    QVariantList args = keyValues.values();
    cleanArguments(args);

    m_sql.setQuery( QString("INSERT INTO %1 (%2) VALUES (%3)").arg(table).arg( QStringList( keyValues.keys() ).join(",") ).arg( TextUtils::getPlaceHolders( keyValues.size(), false ) ) );
    DataAccessReply dar = m_sql.executeAndWait(args);

    if ( dar.hasError() ) {
        emit error( dar.errorMessage() );
    } else {
        m_sql.setQuery("SELECT last_insert_rowid() AS id");
        dar = m_sql.executeAndWait(QVariant());
        QVariantList resultSet = dar.result().toList();

        if ( !resultSet.isEmpty() ) {
            id = resultSet.first().toMap().value("id").toLongLong();
        }
    }

    LOGGER(id);

    return id;
}


void DatabaseHelper::executeUpdate(QObject* caller, QString const& table, QVariantMap const& keyValues, int type, qint64 id, QString const& idField) {
    executeQuery( caller, QString("UPDATE %1 SET %2=? WHERE %3=%4").arg(table).arg( QStringList( keyValues.keys() ).join("=?, ") ).arg(idField).arg(id), type, keyValues.values() );
}


void DatabaseHelper::executeDelete(QObject* caller, QString const& table, int type, qint64 id, QString const& idField) {
    executeQuery( caller, QString("DELETE FROM %1 WHERE %2=%3").arg(table).arg(idField).arg(id), type );
}

void DatabaseHelper::executeDelete(QObject* caller, QString const& table, int type) {
    executeQuery( caller, QString("DELETE FROM %1").arg(table), type );
}


void DatabaseHelper::initSetup(QObject* caller, QStringList const& setupStatements, int t)
{
    stash(caller, t);

    CHECK_ENABLED;

    m_sql.initSetup(setupStatements, m_currentId);
}


void DatabaseHelper::onDestroyed(QObject* obj)
{
    QSet<int> ids = m_objectToIds[obj];
    m_objectToIds.remove(obj);

    foreach (int i, ids) {
        m_idToObjectQueryType.remove(i);
    }
}


void DatabaseHelper::startTransaction(QObject* caller, int id)
{
    if (caller) {
        stash(caller, id);
    }

    CHECK_ENABLED;

    m_sql.startTransaction(m_currentId);
}


void DatabaseHelper::endTransaction(QObject* caller, int id)
{
    if (caller) {
        stash(caller, id);
    }

    CHECK_ENABLED;

    m_sql.endTransaction(m_currentId);
}


void DatabaseHelper::createDatabaseIfNotExists(bool sameThread, QStringList const& setupStatements) const
{
    CHECK_ENABLED;

    if ( !QFile::exists( m_sql.source() ) )
    {
        if (sameThread) {
            createDB( m_sql.source(), setupStatements );
        } else {
            QtConcurrent::run( createDB, m_sql.source(), setupStatements );
        }
    }
}


void DatabaseHelper::setEnabled(bool enabled) {
    m_enabled = enabled;
}


void DatabaseHelper::setVerboseLogging(bool enabled) {
    m_sql.setVerbose(enabled);
}


DatabaseHelper::~DatabaseHelper()
{
}

} /* namespace canadainc */
