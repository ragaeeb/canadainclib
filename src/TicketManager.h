#ifndef TICKETMANAGER_H_
#define TICKETMANAGER_H_

#include <QObject>
#include <QSet>
#include <QVariant>

namespace canadainc {

class TicketManager : public QObject
{
    Q_OBJECT

    int m_currentId;
    QMap< QObject*, QSet<int> > m_objectToIds;
    QMap<int, QPair<QObject*,int> > m_idToObjectQueryType;

private slots:
    void onDestroyed(QObject* obj);

public:
    TicketManager();
    virtual ~TicketManager();

    /**
     * @return The last ticket ID issued.
     */
    int currentId() const;

    /**
     * Invokes the caller associated with the specified ticket.
     * @param ticketId The ticket ID that was generated from the stash() method.
     * @param result The result you want to invoke the object interested in this value with.
     * @return The queryId that was originally associated with the request.
     */
    int drop(int ticketId, QVariant const& data);

    /**
     * @param caller
     * @param queryId
     * @return A ticketID for this specific transaction to use.
     */
    int stash(QObject* caller, int queryId);

    template<class Enum>
    static QString e2s(int t) {
        return QString( Enum::staticMetaObject.enumerator(0).valueToKey(t) );
    }

    template<class Enum>
    static QString s2e(QString const& t)
    {
        QByteArray qba = t.toUtf8();
        return QString( Enum::staticMetaObject.enumerator(0).keyToValue( qba.constData() ) );
    }
};

} /* namespace canadainc */

#endif /* TICKETMANAGER_H_ */
