#ifndef PIMUTIL_H_
#define PIMUTIL_H_

#include <bb/system/InvokeManager>

#include <bb/pim/message/Attachment>

#define ACCOUNT_KEY_SMS 23
#define ACCOUNT_KEY_PIN 199

namespace bb {
	namespace pim {
		namespace message {
			class Message;
			class MessageService;
		}
	}
}

namespace canadainc {

using namespace bb::pim::message;
using namespace bb::system;

class PimUtil : public QObject
{
	Q_OBJECT

public:
	static QString extractText(Message const& m);
	Q_INVOKABLE static void openEmail(qint64 accountId, qint64 messageId);
	Q_INVOKABLE static void openSMSMessage(QString const& conversationKey, qint64 messageId);
	Q_INVOKABLE static void launchPhoneCall(QString const& number);
	Q_INVOKABLE static void replyToSMS(QString const& senderAddress, QString const& body, InvokeManager& invokeManager);
	Q_INVOKABLE static bool validateCalendarAccess(QString const& message, bool launchAppPermissions=true);
	Q_INVOKABLE static bool validateContactsAccess(QString const& message, bool launchAppPermissions=true);
	static qint64 sendMessage(MessageService* ms, Message const& m, QString text, QList<Attachment> const& attachments=QList<Attachment>(), bool replyPrefix=false);
};

} /* namespace canadainc */
#endif /* PIMUTIL_H_ */
