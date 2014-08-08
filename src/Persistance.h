#ifndef PERSISTANCE_H_
#define PERSISTANCE_H_

#include <QSettings>

#include <bb/system/SystemUiResult>

#define INIT_SETTING(a,b) if ( !m_persistance.contains(a) ) m_persistance.saveValueFor(a,b,false);
#define INIT_FRESH(a) if ( !m_persistance.contains(a) ) {m_persistance.clear(); m_persistance.saveValueFor(a, true, false);}

namespace bb {
	namespace system {
		class SystemToast;
	}
}

namespace canadainc {

/**
 * @version 1.00 System toast with OK button and signal emitted.
 */
class Persistance : public QObject
{
	Q_OBJECT

	QSettings m_settings;
	QMap<QString, QVariant> m_pending;
	bb::system::SystemToast* m_toast;

private slots:
    void cacheCleared();
    void commit();

signals:
	void settingChanged(QString const& key);
	void toastFinished(bool buttonTriggered=false);

public:
	Persistance(QObject* parent=NULL);
	virtual ~Persistance();

    Q_INVOKABLE QVariant getValueFor(QString const& objectName) const;
    Q_INVOKABLE bool saveValueFor(QString const& objectName, QVariant const& inputValue, bool fireEvent=true);
    Q_INVOKABLE bool contains(QString const& key);
    Q_INVOKABLE void remove(QString const& key, bool fireEvent=true);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void copyToClipboard(QString const& text, bool showToastMessage=true);
    Q_SLOT void reviewApp(bool prompt=false, QString const& key="promptReview", QString const& message=tr("If you enjoy the app, we would really appreciate if you left us a review! It should only take a second. Would you like to leave one?"));
    Q_SLOT void openChannel(bool promote=false);
    Q_SLOT void donate(QString const& uri="https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=dar.as.sahaba@hotmail.com&currency_code=CAD&no_shipping=1&tax=0&lc=CA&bn=PP-DonationsBF&item_name=Da'wah Activities, Rent and Utility Expenses for the Musalla (please do not use credit cards)");
    Q_INVOKABLE void showToast(QString const& text, QString const& buttonLabel=QString(), QString const& icon=QString());
    Q_INVOKABLE bool tutorial(QString const& key, QString const& message, QString const& icon);
    Q_INVOKABLE bool tutorialVideo(QString const& uri, bool prompt=true, QString const& key="tutorialVideo", QString const& message=tr("Would you like to see a video tutorial on how to use the app?"));

    /**
     * @param return <code>true</code> if the user selected the button to dismiss the toast, <code>false</code> otherwise.
     */
    Q_INVOKABLE static bool showBlockingToast(QString const& text, QString const& buttonLabel=QString(), QString const& icon=QString());
    Q_INVOKABLE static bool showBlockingDialog(QString const& title, QString const& text, QString const& okButton=tr("Yes"), QString const& cancelButton=tr("No"));
    Q_INVOKABLE static QByteArray convertToUtf8(QString const& text);
    Q_SLOT bool clearCache();
};

} /* namespace canadainc */
#endif /* PERSISTANCE_H_ */
