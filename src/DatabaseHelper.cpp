#include "DatabaseHelper.h"

#define ATTACH_DATABASE_ID -1

namespace canadainc {

DatabaseHelper::DatabaseHelper(QString const& dbase, QObject* parent) :
        QObject(parent), m_currentId(0)
{
    m_sql.setSource(dbase);
    connect( &m_sql, SIGNAL( dataLoaded(int, QVariant const&) ), this, SLOT( dataLoaded(int, QVariant const&) ), Qt::QueuedConnection );
}


void DatabaseHelper::attachIfNecessary(QString const& dbase)
{
    if ( !m_attached.contains(dbase) )
    {
        QString path = QString("%1/app/native/assets/dbase/%2.db").arg( QDir::currentPath() ).arg(dbase);
        m_sql.setQuery( QString("ATTACH DATABASE '%1' AS %2").arg(path).arg(dbase) );
        m_sql.load(ATTACH_DATABASE_ID);
        m_attached.insert(dbase, true);
    }
}


void DatabaseHelper::dataLoaded(int id, QVariant const& data)
{
    //LOGGER(id);
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

        //LOGGER("Emitting data loaded" << t << caller);

        //LOGGER("DATA" << data);
        QMetaObject::invokeMethod(caller, "onDataLoaded", Qt::QueuedConnection, Q_ARG(QVariant, t), Q_ARG(QVariant, data) );
    }
}


void DatabaseHelper::stash(QObject* caller, int t)
{
    ++m_currentId;

    //LOGGER(caller << query << t << m_currentId);

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
    stash(caller, t);

    m_sql.setQuery(query);

    if ( args.isEmpty() ) {
        m_sql.load(m_currentId);
    } else {
        m_sql.executePrepared(args, m_currentId);
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


DatabaseHelper::~DatabaseHelper()
{
}

} /* namespace canadainc */
