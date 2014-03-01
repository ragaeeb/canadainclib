#ifndef PIMUTIL_H_
#define PIMUTIL_H_

#include <QObject>
#include <QString>

#include <bb/system/InvokeManager>

namespace bb {
	namespace pim {
		namespace message {
			class Message;
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
};

} /* namespace canadainc */
#endif /* PIMUTIL_H_ */
