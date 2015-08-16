#ifndef PERSISTANCE_H_
#define PERSISTANCE_H_

#include <QSettings>

#include <bb/system/InvokeManager>
#include <bb/system/SystemUiPosition>
#include <bb/system/SystemUiResult>

#include "DialogUtils.h"

#define INIT_SETTING(a,b) if ( !m_persistance.contains(a) ) m_persistance.setValueInternal(a,b);
#define FLAGS_FILE_NAME "flags"

namespace bb {
	namespace system {
	    class SystemDialog;
	    class SystemPrompt;
		class SystemToast;
	}
}

namespace canadainc {

class Persistance : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool isBlocked READ isBlocked FINAL)

    bb::system::InvokeManager* m_invokeManager;
    QMap<QString, bool> m_logMap;
    QMap<QString, QVariant> m_pending;
    QSettings m_flags;
    QSettings m_settings;
    QMap<QString, QObjectList> m_settingToListeners;
    QMap<QObject*, QStringList> m_listenerToSettings;
    QMap<QObject*, QObject*> m_destroyWatchers;
    DialogUtils m_dialogs;

private slots:
    void commit();
    void onDestroyed(QObject* obj);
    void onInvokeFinished();
    void onTargetDestroyed(QObject* obj);

signals:
	void settingChanged(QString const& key);

public:
	Persistance(bb::system::InvokeManager* im);
	virtual ~Persistance();

    bb::system::InvokeManager* invokeManager();
    bool isBlocked() const;
    void portLegacy(QStringList settingKeys);
    Q_INVOKABLE bool contains(QString const& key) const;
    Q_INVOKABLE bool containsFlag(QString const& key);
    Q_INVOKABLE void expose(QString const& key, QObject* q);
    Q_INVOKABLE bool isUpdateNeeded(QString const& key, int diffDaysMin=30);
    Q_INVOKABLE bool saveValueFor(QString const& objectName, QVariant const& inputValue, bool fireEvent=true);
    Q_INVOKABLE QString getClipboardText() const;
    Q_INVOKABLE QVariant getFlag(QString const& key);
    Q_INVOKABLE QVariant getValueFor(QString const& objectName);
    Q_INVOKABLE static bool hasEmailSmsAccess();
    Q_INVOKABLE static bool hasLocationAccess();
    Q_INVOKABLE static bool hasPhoneControlAccess();
    Q_INVOKABLE static bool hasSharedFolderAccess();
    Q_INVOKABLE static bool showBlockingDialog(QString const& title, QString const& text, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"), bool okEnabled=true);
    Q_INVOKABLE static QByteArray convertToUtf8(QString const& text);
    Q_INVOKABLE static QString homePath();
    Q_INVOKABLE static QString showBlockingPrompt(QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize=true, QString const& okButton=tr("OK"), QString const& cancelButton=tr("Cancel"), int inputMode=0);
    Q_INVOKABLE static QString tempPath();
    Q_INVOKABLE void call(QString const& number);
    Q_INVOKABLE void copyToClipboard(QString const& text, bool showToastMessage=true);
    Q_INVOKABLE void findTarget(QString const& uri, QString const& target, QObject* caller);
    Q_INVOKABLE void invoke(QString const& targetId, QString const& action="", QString const& mime="", QString const& uri="", QString const& data="", QObject* callback=NULL);
    Q_INVOKABLE void launchAppPermissionSettings();
    Q_INVOKABLE void launchSettingsApp(QString const& key, QVariantMap const& metadata=QVariantMap());
    Q_INVOKABLE void openBlackBerryWorld(QString const& appID);

    /**
     *Note that we don't currently monitor when the caller is destroyed. So make sure the caller is alive forever otherwise there
     *will be leaks.
     */
    Q_INVOKABLE void registerForDestroyed(QObject* toWatch, QObject* caller);
    Q_INVOKABLE void registerForSetting(QObject* q, QString const& key, bool isFlag=false, bool immediate=true);
    Q_INVOKABLE void remove(QString const& key, bool fireEvent=true);
    Q_INVOKABLE void setFlag(QString const& key, QVariant const& value=QVariant());
    Q_INVOKABLE void showDialog(QObject* caller, QString const& title, QString const& text, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"), QString const& rememberMeText=QString(), bool rememberMeValue=false, QString const& funcName="onFinished");
    Q_INVOKABLE void showDialog(QObject* caller, QVariant const& data, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled=true, QString const& rememberMeText=QString(), bool rememberMeValue=false, QString const& funcName="onFinished");
    Q_INVOKABLE void showDialog(QString const& title, QString const& text, QString okButton=tr("OK"));
    Q_INVOKABLE void showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize=true, QString const& okButton=tr("Save"), QString const& cancelButton=tr("Cancel"), int inputMode=0, QString const& funcName="onFinished", QVariant const& data=QVariant());
    Q_INVOKABLE void showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, QString const& funcName, QVariant const& data=QVariant());
    Q_INVOKABLE void showToast(QString const& text, QString const& icon=QString(), int pos=2);
    Q_SLOT void clear();
    Q_SLOT void donate();
    Q_SLOT void forceSync();
    Q_SLOT void openChannel();
    Q_SLOT void openUri(QString const& uri);
    Q_SLOT void reviewApp();
    void setValueInternal(QString const& key, QVariant const& value);
    static bool showBlockingDialog(QString const& title, QString const& text, QString const& rememberMeText, bool &rememberMeValue, QString const& okButton, QString const& cancelButton, bool okEnabled=true);
    static void onErrorMessage(const char* msg);
    Q_SLOT void onError(QString const& message);
};

} /* namespace canadainc */
#endif /* PERSISTANCE_H_ */
