#ifndef PERSISTANCE_H_
#define PERSISTANCE_H_

#include <QSettings>

#include <bb/system/InvokeManager>
#include <bb/system/SystemUiPosition>
#include <bb/system/SystemUiResult>

#define INIT_SETTING(a,b) if ( !m_persistance.contains(a) ) m_persistance.saveValueFor(a,b,false);
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
	Q_PROPERTY(bool isBlocked READ isBlocked NOTIFY isBlockedChanged)

    bb::system::InvokeManager m_invokeManager;
    bb::system::SystemDialog* m_dialog;
    bb::system::SystemToast* m_toast;
    bb::system::SystemPrompt* m_prompt;
    QMap<QString, bool> m_logMap;
    QMap<QString, QVariant> m_pending;
    QSettings m_flags;
    QSettings m_settings;

private slots:
    void cacheCleared();
    void commit();
    void dialogFinished(bb::system::SystemUiResult::Type value);
    void finished(bb::system::SystemUiResult::Type value);
    void onDestroyed(QObject* obj);
    void promptFinished(bb::system::SystemUiResult::Type value);

signals:
    void isBlockedChanged();
	void settingChanged(QString const& key);

public:
	Persistance(QObject* parent=NULL);
	virtual ~Persistance();

    bb::system::InvokeManager* invokeManager();
    bool isBlocked() const;
    Q_INVOKABLE bool contains(QString const& key) const;
    Q_INVOKABLE bool containsFlag(QString const& key);
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
    Q_INVOKABLE static QString showBlockingPrompt(QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize=true, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"), int inputMode=0);
    Q_INVOKABLE void attachBackKeyToClickedSignal(QObject* abstractButton, QObject* rootControl);
    Q_INVOKABLE void call(QString const& number);
    Q_INVOKABLE void copyToClipboard(QString const& text, bool showToastMessage=true);
    Q_INVOKABLE void invoke(QString const& targetId, QString const& action="", QString const& mime="", QString const& uri="", QString const& data="");
    Q_INVOKABLE void launchAppPermissionSettings();
    Q_INVOKABLE void launchSettingsApp(QString const& key, QVariantMap const& metadata=QVariantMap());
    Q_INVOKABLE void openBlackBerryWorld(QString const& appID);
    Q_INVOKABLE void remove(QString const& key, bool fireEvent=true);
    Q_INVOKABLE void setFlag(QString const& key, QVariant const& value=QVariant());
    Q_INVOKABLE void showDialog(QObject* caller, QString const& title, QString const& text, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"), QString const& rememberMeText=QString(), bool rememberMeValue=false);
    Q_INVOKABLE void showDialog(QObject* caller, QVariant const& data, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled=true, QString const& rememberMeText=QString(), bool rememberMeValue=false);
    Q_INVOKABLE void showDialog(QString const& title, QString const& text, QString okButton=tr("OK"));
    Q_INVOKABLE void showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize=true, QString const& okButton=tr("Save"), QString const& cancelButton=tr("Cancel"), int inputMode=0, QString const& funcName="onFinished", QVariant const& data=QVariant());
    Q_INVOKABLE void showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, QString const& funcName, QVariant const& data=QVariant());
    Q_INVOKABLE void showToast(QString const& text, QString const& icon=QString(), bb::system::SystemUiPosition::Type pos=bb::system::SystemUiPosition::BottomCenter);
    Q_SLOT bool clearCache();
    Q_SLOT void clear();
    Q_SLOT void forceSync();
    Q_SLOT void openChannel();
    Q_SLOT void openUri(QString const& uri);
    Q_SLOT void reviewApp();
    static bool showBlockingDialog(QString const& title, QString const& text, QString const& rememberMeText, bool &rememberMeValue, QString const& okButton, QString const& cancelButton, bool okEnabled=true);
};

} /* namespace canadainc */
#endif /* PERSISTANCE_H_ */
