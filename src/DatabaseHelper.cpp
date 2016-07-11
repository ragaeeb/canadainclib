#include "DatabaseHelper.h"
#include "IOUtils.h"
#include "Logger.h"

#include <QCoreApplication>
#include <QDir>
#include <QtCore>

#define CHECK_ENABLED if (!m_enabled) return

namespace {

QPair<QObject*, QStringList> createDB(QString const& dbPath, QObject* caller, QStringList const& setupStatements)
{
    canadainc::IOUtils::writeFile(dbPath);
    return qMakePair<QObject*, QStringList>(caller, setupStatements);
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
        QObject(parent), m_enabled(true)
{
    m_sql.setSource(dbase);
    connect( &m_sql, SIGNAL( dataLoaded(int, QVariant const&) ), this, SLOT( dataLoaded(int, QVariant const&) ), Qt::QueuedConnection );
    connect( &m_sql, SIGNAL( error(QString const&) ), this, SIGNAL( error(QString const&) ), Qt::QueuedConnection );
    connect( &m_sql, SIGNAL( setupError(QString const&) ), this, SIGNAL( setupError(QString const&) ), Qt::QueuedConnection );
    connect( &m_setup, SIGNAL( finished() ), this, SLOT( onSetupFinished() ) );
}


void DatabaseHelper::onDatabaseCreated()
{
}


void DatabaseHelper::attachIfNecessary(QString const& dbase, bool homePath) {
    attachIfNecessary( dbase, homePath ? QDir::homePath() : QString("%1/assets/dbase").arg( QCoreApplication::applicationDirPath() ) );
}


void DatabaseHelper::attachIfNecessary(QString const& dbase, QString const& path)
{
    CHECK_ENABLED;

    if ( !dbase.isEmpty() && !m_attached.contains(dbase) )
    {
        m_sql.setQuery( QString("ATTACH DATABASE '%1' AS %2").arg( QString("%1/%2.db") ).arg(path).arg(dbase) );
        m_sql.load(InternalQueryId::AttachDatabase);
        m_attached << dbase;
    }
}


void DatabaseHelper::detach(QString const& dbase)
{
    CHECK_ENABLED;

    if ( m_attached.contains(dbase) )
    {
        m_sql.setQuery( QString("DETACH DATABASE %1").arg(dbase) );
        m_sql.load(InternalQueryId::DetachDatabase);
        m_attached.remove(dbase);
    }
}


void DatabaseHelper::enableForeignKeys()
{
    CHECK_ENABLED;

    m_sql.setQuery("PRAGMA foreign_keys = ON");
    m_sql.load(InternalQueryId::ForeignKeySetup);
}


void DatabaseHelper::dataLoaded(int id, QVariant const& data)
{
    id = m_ticket.drop(id, data);
    emit finished(id);
}


void DatabaseHelper::executeQuery(QObject* caller, QString const& query, int t, QVariantList const& args)
{
    if (caller) {
        m_ticket.stash(caller, t);
    }

    executeInternal(query, m_ticket.currentId(), args);
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

    m_sql.setQuery( QString("INSERT INTO %1 (%2) VALUES (%3)").arg(table).arg( QStringList( keyValues.keys() ).join(",") ).arg( getPlaceHolders( keyValues.size(), false ) ) );
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

void DatabaseHelper::executeClear(QObject* caller, QString const& table, int type) {
    executeQuery( caller, QString("DELETE FROM %1").arg(table), type );
}


void DatabaseHelper::startTransaction(QObject* caller, int id)
{
    if (caller) {
        m_ticket.stash(caller, id);
    }

    CHECK_ENABLED;

    m_sql.startTransaction( m_ticket.currentId() );
}


void DatabaseHelper::endTransaction(QObject* caller, int id)
{
    if (caller) {
        m_ticket.stash(caller, id);
    }

    CHECK_ENABLED;

    m_sql.endTransaction( m_ticket.currentId() );
}


bool DatabaseHelper::createDatabaseIfNotExists(QObject* caller, QStringList const& setupStatements)
{
    if ( !QFile::exists( m_sql.source() ) )
    {
        if (!caller)
        {
            createDB( m_sql.source(), caller, setupStatements );
            processSetup(caller, setupStatements);
        } else {
            QFuture< QPair<QObject*, QStringList> > f = QtConcurrent::run( createDB, m_sql.source(), caller, setupStatements );
            m_setup.setFuture(f);
        }

        return false;
    }

    return true;
}


void DatabaseHelper::processSetup(QObject* caller, QStringList const& statements)
{
    startTransaction(NULL, InternalQueryId::SettingUp);

    foreach (QString const& query, statements) {
        executeInternal(query, InternalQueryId::SettingUp);
    }

    endTransaction(caller, InternalQueryId::Setup);
}


void DatabaseHelper::onSetupFinished()
{
    QPair<QObject*, QStringList> result = m_setup.result();
    processSetup(result.first, result.second);
}


void DatabaseHelper::setEnabled(bool enabled) {
    m_enabled = enabled;
}


void DatabaseHelper::setVerboseLogging(bool enabled) {
    m_sql.setVerbose(enabled);
}


QString DatabaseHelper::getPlaceHolders(int n, bool multi, QString const& symbol)
{
    QStringList placeHolders;

    for (int i = 0; i < n; i++) {
        placeHolders << symbol;
    }

    return placeHolders.join(multi ? "),(" : ",");
}


DatabaseHelper::~DatabaseHelper()
{
}

} /* namespace canadainc */
