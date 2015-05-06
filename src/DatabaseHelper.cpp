#include "DatabaseHelper.h"

#include <QCoreApplication>
#include <QDir>

namespace canadainc {

DatabaseHelper::DatabaseHelper(QString const& dbase, QObject* parent) :
        QObject(parent), m_currentId(0)
{
    m_sql.setSource(dbase);
    connect( &m_sql, SIGNAL( dataLoaded(int, QVariant const&) ), this, SLOT( dataLoaded(int, QVariant const&) ), Qt::QueuedConnection );

    connect( QCoreApplication::instance(), SIGNAL( aboutToQuit() ), this, SLOT( commitStats() ) );
}


void DatabaseHelper::commitStats()
{
    if ( !m_counters.isEmpty() )
    {
        QList< QPair<QString, QString> > keys = m_counters.keys();

        m_sql.startTransaction(COMMITTING_ANALYTICS);
        m_sql.setQuery("CREATE TABLE IF NOT EXISTS analytics.events (id INTEGER PRIMARY KEY, event TEXT, context TEXT, count INTEGER, UNIQUE(event,context) ON CONFLICT REPLACE CHECK(event <> '' AND context <> ''))");
        m_sql.load(COMMITTING_ANALYTICS);

        for (int i = keys.size()-1; i >= 0; i--)
        {
            QPair<QString, QString> pair = keys[i];
            m_sql.setQuery("INSERT INTO analytics.events (event,context,count) VALUES (?,?,?)");
            m_sql.executePrepared( QVariantList() << pair.first << pair.second << m_counters.value(pair), COMMITTING_ANALYTICS );
        }

        m_sql.endTransaction(COMMIT_ANALYTICS);
    }
}


void DatabaseHelper::attachIfNecessary(QString const& dbase, bool homePath, int id) {
    attachIfNecessary( dbase, homePath ? QDir::homePath() : QString("%1/assets/dbase").arg( QCoreApplication::applicationDirPath() ), id );
}


void DatabaseHelper::attachIfNecessary(QString const& dbase, QString const& path, int id)
{
    if ( !dbase.isEmpty() && !m_attached.contains(dbase) )
    {
        m_sql.setQuery( QString("ATTACH DATABASE '%1' AS %2").arg( QString("%1/%2.db") ).arg(path).arg(dbase) );
        m_sql.load(id);
        m_attached.insert(dbase, true);
    }
}


void DatabaseHelper::detach(QString const& dbase, int id)
{
    if ( m_attached.contains(dbase) )
    {
        m_sql.setQuery( QString("DETACH DATABASE %1").arg(dbase) );
        m_sql.load(id);
        m_attached.remove(dbase);
    }
}


void DatabaseHelper::enableForeignKeys(int id)
{
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

        QMap<int,bool> idsForObject = m_objectToIds[caller];
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

    QMap<int,bool> idsForObject = m_objectToIds[caller];
    idsForObject.insert(m_currentId, true);
    m_objectToIds[caller] = idsForObject;
}


void DatabaseHelper::executeQuery(QObject* caller, QString const& query, int t, QVariantList const& args)
{
    if (caller) {
        stash(caller, t);
    }

    executeInternal(query, m_currentId, args);
}


void DatabaseHelper::executeInternal(QString const& query, int t, QVariantList const& args)
{
    m_sql.setQuery(query);

    if ( args.isEmpty() ) {
        m_sql.load(t);
    } else {
        m_sql.executePrepared(args, t);
    }
}


void DatabaseHelper::initSetup(QObject* caller, QStringList const& setupStatements, int t)
{
    stash(caller, t);

    m_sql.initSetup(setupStatements, m_currentId);
}


void DatabaseHelper::onDestroyed(QObject* obj)
{
    QMap<int,bool> idsForObject = m_objectToIds[obj];
    m_objectToIds.remove(obj);

    QList<int> ids = idsForObject.keys();

    for (int i = ids.size()-1; i >= 0; i--) {
        m_idToObjectQueryType.remove(ids[i]);
    }
}


void DatabaseHelper::startTransaction(QObject* caller, int id)
{
    if (caller) {
        stash(caller, id);
    }

    m_sql.startTransaction(m_currentId);
}


void DatabaseHelper::endTransaction(QObject* caller, int id)
{
    if (caller) {
        stash(caller, id);
    }

    m_sql.endTransaction(m_currentId);
}


void DatabaseHelper::record(QString const& event, QString const& context)
{
    attachIfNecessary("analytics", true);

    QPair<QString, QString> pair = qMakePair<QString, QString>(event, context);
    int count = m_counters.value(pair);
    ++count;

    m_counters[pair] = count;
}


DatabaseHelper::~DatabaseHelper()
{
}

} /* namespace canadainc */
