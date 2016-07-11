#include "TicketManager.h"

namespace canadainc {

TicketManager::TicketManager() : m_currentId(0)
{
}


void TicketManager::onDestroyed(QObject* obj)
{
    QSet<int> ids = m_objectToIds[obj];
    m_objectToIds.remove(obj);

    foreach (int i, ids) {
        m_idToObjectQueryType.remove(i);
    }
}


int TicketManager::drop(int ticketId, QVariant const& result)
{
    if ( m_idToObjectQueryType.contains(ticketId) )
    {
        QPair<QObject*, int> value = m_idToObjectQueryType[ticketId];
        QObject* caller = value.first;
        int t = value.second;

        m_idToObjectQueryType.remove(ticketId);

        QSet<int> idsForObject = m_objectToIds[caller];
        idsForObject.remove(ticketId);

        if ( !idsForObject.isEmpty() ) {
            m_objectToIds[caller] = idsForObject;
        } else {
            m_objectToIds.remove(caller);
        }

        QMetaObject::invokeMethod(caller, "onDataLoaded", Qt::QueuedConnection, Q_ARG(QVariant, t), Q_ARG(QVariant, result) );

        return t;
    }

    return 0;
}


int TicketManager::stash(QObject* caller, int t)
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

    return m_currentId;
}


int TicketManager::currentId() const {
    return m_currentId;
}


TicketManager::~TicketManager()
{
}

} /* namespace canadainc */
