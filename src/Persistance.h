#ifndef PERSISTANCE_H_
#define PERSISTANCE_H_

#include <QSettings>

#include <bb/system/SystemUiPosition>
#include <bb/system/SystemUiResult>

#define INIT_SETTING(a,b) if ( !m_persistance.contains(a) ) m_persistance.saveValueFor(a,b,false);
#define INIT_FRESH(a) if ( !m_persistance.contains(a) ) {m_persistance.clear(); m_persistance.saveValueFor(a, true, false);}

namespace bb {
	namespace system {
	    class SystemDialog;
		class SystemToast;
	}
}

namespace canadainc {

class Persistance : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool isBlocked READ isBlocked NOTIFY isBlockedChanged)

	QSettings m_settings;
	QMap<QString, QVariant> m_pending;
	bb::system::SystemToast* m_toast;
	bb::system::SystemDialog* m_dialog;
	QMap<QString, bool> m_logMap;

private slots:
    void cacheCleared();
    void commit();
    void dialogFinished(bb::system::SystemUiResult::Type value);
    void finished(bb::system::SystemUiResult::Type value);
    void onDestroyed(QObject* obj);

signals:
    void isBlockedChanged();
	void settingChanged(QString const& key);

public:
	Persistance(QObject* parent=NULL);
	virtual ~Persistance();

    bool isBlocked() const;
    Q_INVOKABLE bool contains(QString const& key) const;
    Q_INVOKABLE bool saveValueFor(QString const& objectName, QVariant const& inputValue, bool fireEvent=true);
    Q_INVOKABLE bool tutorial(QString const& key, QString const& message, QString const& icon);
    Q_INVOKABLE bool tutorialVideo(QString const& uri, bool prompt=true, QString const& key="tutorialVideo", QString const& message=tr("Would you like to see a video tutorial on how to use the app?"));
    Q_INVOKABLE QString getClipboardText() const;
    Q_INVOKABLE QVariant getValueFor(QString const& objectName);
    Q_INVOKABLE QVariantList showBlockingDialogWithRemember(QString const& title, QString const& text, QString const& rememberMeText, bool rememberMeValue=false, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"));
    Q_INVOKABLE static bool hasEmailSmsAccess();
    Q_INVOKABLE static bool hasLocationAccess();
    Q_INVOKABLE static bool hasPhoneControlAccess();
    Q_INVOKABLE static bool hasSharedFolderAccess();
    Q_INVOKABLE static bool showBlockingDialog(QString const& title, QString const& text, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"), bool okEnabled=true);


    Q_INVOKABLE void showDialog(QObject* caller, QVariant const& data, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled=true, QString const& rememberMeText=QString(), bool rememberMeValue=false);
    Q_INVOKABLE void showDialog(QString const& title, QString const& text, QString okButton=tr("OK"));
    Q_INVOKABLE void showDialog(QObject* caller, QString const& title, QString const& text, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"), QString const& rememberMeText=QString(), bool rememberMeValue=false);


    Q_INVOKABLE static bool showBlockingToast(QString const& text, QString const& icon=QString());
    Q_INVOKABLE static QByteArray convertToUtf8(QString const& text);
    Q_INVOKABLE static QString showBlockingPrompt(QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize=true, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"), int inputMode=0);
    Q_INVOKABLE static void launchAppPermissionSettings();
    Q_INVOKABLE static void openBlackBerryWorld(QString const& appID);
    Q_INVOKABLE void attachBackKeyToClickedSignal(QObject* abstractButton, QObject* rootControl);
    Q_INVOKABLE void copyToClipboard(QString const& text, bool showToastMessage=true);
    Q_INVOKABLE void invoke(QString const& targetId, QString const& action="", QString const& mime="", QString const& uri="", QString const& data="");
    Q_INVOKABLE void remove(QString const& key, bool fireEvent=true);
    Q_INVOKABLE void showToast(QString const& text, QString const& icon=QString(), bb::system::SystemUiPosition::Type pos=bb::system::SystemUiPosition::BottomCenter);
    Q_SLOT bool clearCache();
    Q_SLOT bool reviewed();
    Q_SLOT void clear();
    Q_SLOT void donate(QString const& uri="https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=dar.as.sahaba@hotmail.com&currency_code=CAD&no_shipping=1&tax=0&lc=CA&bn=PP-DonationsBF&item_name=Da'wah Activities, Rent and Utility Expenses for the Musalla (please do not use credit cards)");
    Q_SLOT void forceSync();
    Q_SLOT void openChannel(bool promote=false);
    Q_SLOT void reviewApp();
    Q_INVOKABLE bool isUpdateNeeded(QString const& key, int diffDaysMin=30);
    static bool showBlockingDialog(QString const& title, QString const& text, QString const& rememberMeText, bool &rememberMeValue, QString const& okButton, QString const& cancelButton, bool okEnabled=true);
};

} /* namespace canadainc */
#endif /* PERSISTANCE_H_ */
