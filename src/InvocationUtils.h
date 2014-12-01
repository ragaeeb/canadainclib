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
    Q_INVOKABLE static void launchBBMCall(QString const& pin, bool videoEnabled=true);
    static void launchBBMChat(QString const& pin, InvokeManager& invokeManager);
    static void launchBrowser(QString const& uri);
    static void launchDoc(QString const& uri, InvokeManager& invokeManager);
    static void launchEmailComposer(QString const& address, InvokeManager& invokeManager);
    Q_INVOKABLE static void launchLocationServices();
    static void launchPhoto(QString const& uri, InvokeManager* invokeManager);
    Q_INVOKABLE static void launchSettingsApp(QString const& key);
    static void launchSMSComposer(QString const& number, InvokeManager& invokeManager);
	static void replyToMessage(qint64 accountId, QString const& messageId, InvokeManager& invokeManager);
};

} /* namespace canadainc */
#endif /* INVOCATIONUTILS_H_ */
