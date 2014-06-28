#ifndef MESSAGEIMPORTER_H_
#define MESSAGEIMPORTER_H_

#include <QObject>
#include <QRunnable>
#include <QVariantList>

namespace bb {
	namespace pim {
		namespace message {
			class Message;
		}
	}
}

namespace canadainc {

using namespace bb::pim::message;

class MessageImporter : public QObject, public QRunnable
{
	Q_OBJECT

	qint64 m_accountKey;
	bool m_inboundOnly;
	bool m_latestFirst;
	QString m_userAlias;
	QString m_conversationKey;
	bool m_unreadOnly;
	bool m_deviceTime;
	int m_timeLimit;
	bool m_quit;

	void appendIfValid(Message const& m, QVariantList& variants);
	void processConversation();
	QVariantList processAllConversations();
	QVariantList processSingleConversation();
	QVariantList fetchCalls();

signals:
	/**
	 * Emitted once all the SMS messages have been imported.
	 * @param qvl A list of QVariantMap objects. Each entry has a key for the conversation ID, and a name of the contact it is
	 * associated with.
	 */
	void importCompleted(QVariantList const& qvl);
	void progress(int current, int total);

public:
	MessageImporter(qint64 accountKey, bool onlyInbound=true);
	~MessageImporter();
	QVariantList getResult();
	Q_SLOT void cancel();
	void run();
	void setUserAlias(QString const& alias);
	void setConversation(QString const& conversationKey);
	void setLatestFirst(bool latestFirst=true);
	void setUnreadOnly(bool unreadOnly=true);
	void setUseDeviceTime(bool deviceTime=true);
	void setTimeLimit(int days=INT_MAX);
	static QVariantMap transform(Message const& m);
};

} /* namespace canadainc */
#endif /* MESSAGEIMPORTER_H_ */
