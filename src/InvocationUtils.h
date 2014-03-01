#ifndef INVOCATIONUTILS_H_
#define INVOCATIONUTILS_H_

#include <QObject>
#include <QVariantMap>

#include <bb/system/InvokeManager>

namespace bb {
	namespace system {
		class InvokeManager;
	}
}

namespace canadainc {

using namespace bb::system;

class InvocationUtils : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE static void launchAppPermissionSettings();
	Q_INVOKABLE static void replyToMessage(qint64 accountId, QString const& messageId, InvokeManager& invokeManager);
	Q_INVOKABLE static bool validateSharedFolderAccess(QString const& message, bool launchAppPermissions=true);
	Q_INVOKABLE static bool validateEmailSMSAccess(QString const& message, bool launchAppPermissions=true);
	Q_INVOKABLE static bool validateLocationAccess(QString const& message, bool launchAppPermissions=true);
	Q_INVOKABLE static void launchLocationServices();
	Q_INVOKABLE static void launchBBM();
	Q_INVOKABLE static void launchBBMChat(QString const& pin, InvokeManager& invokeManager);
	Q_INVOKABLE static void launchBBMCall(QString const& pin, bool videoEnabled=true);
	Q_INVOKABLE static void launchEmailComposer(QString const& address, InvokeManager& invokeManager);
	Q_INVOKABLE static void launchSMSComposer(QString const& number, InvokeManager& invokeManager);
	Q_INVOKABLE static void launchPhoto(QString const& uri, InvokeManager* invokeManager);
	Q_INVOKABLE static void launchDoc(QString const& uri, InvokeManager& invokeManager);
	Q_INVOKABLE static void launchAudio(QString const& uri);
	Q_INVOKABLE static void launchSettingsApp(QString const& key);
	static QVariantMap parseArgs(QString const& requestUri);
	static QString encodeArgs(QVariantMap const& map);
};

} /* namespace canadainc */
#endif /* INVOCATIONUTILS_H_ */
