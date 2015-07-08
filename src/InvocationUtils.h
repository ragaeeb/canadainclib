#ifndef INVOCATIONUTILS_H_
#define INVOCATIONUTILS_H_

#include <QVariantMap>

#include <bb/system/InvokeManager>

namespace canadainc {

using namespace bb::system;

class InvocationUtils : public QObject
{
	Q_OBJECT

public:
	InvocationUtils(QObject* parent=NULL);
	virtual ~InvocationUtils();
    Q_INVOKABLE static void launchAudio(QString const& uri);
    Q_INVOKABLE static void launchBBM();
    static void launchBBMChat(QString const& pin, InvokeManager& invokeManager);
    static void launchBrowser(QString const& uri);
    static void launchDoc(QString const& uri, InvokeManager& invokeManager);
    static void launchEmailComposer(QString const& address, InvokeManager& invokeManager);
    static void launchPhoto(QString const& uri, InvokeManager* invokeManager);
    static void launchSMSComposer(QString const& number, InvokeManager& invokeManager);
	static void replyToMessage(qint64 accountId, QString const& messageId, InvokeManager& invokeManager);
	static void call(QString const& number, InvokeManager* invokeManager);
	static InvokeTargetReply* invoke(QString const& targetId, QString const& action, QString const& mime, QString const& uri, QString const& data, InvokeManager* invokeManager);
    static bool hasEmailSmsAccess();
    static bool hasLocationAccess();
    static bool hasPhoneControlAccess();
    static bool hasSharedFolderAccess();
    static void launchSettingsApp(QString const& key, QVariantMap const& metadata, InvokeManager* invokeManager);
};

} /* namespace canadainc */
#endif /* INVOCATIONUTILS_H_ */
