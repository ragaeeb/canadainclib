#include "Persistance.h"
#include "InvocationUtils.h"
#include "IOUtils.h"
#include "Logger.h"

#include <bb/system/Clipboard>
#include <bb/system/InvokeAction>
#include <bb/system/InvokeTargetReply>
#include <bb/system/SystemDialog>
#include <bb/system/SystemPrompt>

#include <bb/cascades/QmlDocument>

#include <bb/PackageInfo>

#include <QCoreApplication>

namespace canadainc {

using namespace bb::cascades;
using namespace bb::system;

Persistance::Persistance(bb::system::InvokeManager* im) :
        m_invokeManager(im), m_flags(FLAGS_FILE_NAME)
{
    QDeclarativeContext* rootContext = QmlDocument::defaultDeclarativeEngine()->rootContext();
    rootContext->setContextProperty("persist", this);

    connect( QCoreApplication::instance(), SIGNAL( aboutToQuit() ), this, SLOT( commit() ) );
}


void Persistance::commit()
{
    QStringList keys = m_pending.keys();

    foreach ( QString const& key, m_pending.keys() ) {
        m_settings.setValue( key, m_pending.value(key) );
    }
}


void Persistance::showToast(QString const& text, QString const& icon, int pos) {
    m_dialogs.showToast(text, icon, pos);
}


void Persistance::findTarget(QString const& uri, QString const& target, QObject* caller) {
    m_dialogs.findTarget(uri, target, caller, m_invokeManager);
}


bool Persistance::showBlockingDialog(QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled)
{
	bool remember = false;
	return DialogUtils::showBlockingDialog(title, text, QString(), remember, okButton, cancelButton, okEnabled);
}


void Persistance::onDestroyed(QObject* obj)
{
    if (obj == m_dialogs.m_dialog) {
        m_dialogs.m_dialog = NULL;
    } else if (obj == m_dialogs.m_prompt) {
        m_dialogs.m_prompt = NULL;
    } else {
        QStringList keys = m_listenerToSettings.value(obj); // all the keys this object was listening to

        foreach (QString const& key, keys)
        {
            if ( !key.isEmpty() )
            {
                QObjectList l = m_settingToListeners.value(key);
                l.removeAll(obj);

                m_settingToListeners.insert(key, l);
                m_listenerToSettings.remove(obj);
            }
        }
    }
}


void Persistance::registerForSetting(QObject* q, QString const& key, bool isFlag, bool immediate)
{
    QObjectList all = m_settingToListeners.value(key);
    all << q;

    QStringList keys = m_listenerToSettings.value(q);
    keys << key;

    m_settingToListeners.insert(key, all);
    m_listenerToSettings.insert(q, keys);

    connect( q, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );

    if (immediate) {
        QMetaObject::invokeMethod( q, "onSettingChanged", Qt::QueuedConnection, Q_ARG(QVariant, isFlag ? getFlag(key) : getValueFor(key) ), Q_ARG(QVariant, key) );
    }
}


void Persistance::registerForDestroyed(QObject* toWatch, QObject* caller)
{
    connect( toWatch, SIGNAL( destroyed(QObject*) ), this, SLOT( onTargetDestroyed(QObject*) ) );
    m_destroyWatchers.insert(toWatch, caller);
}


void Persistance::onTargetDestroyed(QObject* obj)
{
    QObject* watcher = m_destroyWatchers.value(obj);
    m_destroyWatchers.remove(obj);

    QMetaObject::invokeMethod(watcher, "onDestroyed", Qt::QueuedConnection);
}


void Persistance::showDialog(QString const& title, QString const& text, QString okButton) {
    showDialog(NULL, QVariant(), title, text, okButton, "");
}


void Persistance::showDialog(QObject* caller, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, QString const& rememberMeText, bool rememberMeValue) {
    showDialog(caller, QVariant(), title, text, okButton, cancelButton, true, rememberMeText, rememberMeValue);
}


void Persistance::showDialog(QObject* caller, QVariant const& data, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled, QString const& rememberMeText, bool rememberMeValue) {
    m_dialogs.showDialog(caller, data, title, text, okButton, cancelButton, okEnabled, rememberMeText, rememberMeValue);
}


void Persistance::showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize, QString const& okButton, QString const& cancelButton, int inputMode, QString const& funcName, QVariant const& data) {
    m_dialogs.showPrompt(caller, title, body, defaultText, hintText, maxLength, autoCapitalize, okButton, cancelButton, inputMode, funcName, data);
}


void Persistance::showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, QString const& funcName, QVariant const& data) {
    showPrompt(caller, title, body, defaultText, hintText, maxLength, true, tr("Save"), tr("Cancel"), 0, funcName, data);
}


bool Persistance::showBlockingDialog(QString const& title, QString const& text, QString const& rememberMeText, bool &rememberMeValue, QString const& okButton, QString const& cancelButton, bool okEnabled) {
    return DialogUtils::showBlockingDialog(title, text, rememberMeText, rememberMeValue, okButton, cancelButton, okEnabled);
}


QString Persistance::showBlockingPrompt(QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize, QString const& okButton, QString const& cancelButton, int inputMode) {
    return DialogUtils::showBlockingPrompt(title, body, defaultText, hintText, maxLength, autoCapitalize, okButton, cancelButton, inputMode);
}


void Persistance::copyToClipboard(QString const& text, bool showToastMessage)
{
	Clipboard clipboard;
	clipboard.clear();
	clipboard.insert( "text/plain", convertToUtf8(text) );

	if (showToastMessage) {
		showToast( tr("Copied: %1 to clipboard").arg(text), "asset:///images/menu/ic_copy.png" );
	}
}


QString Persistance::getClipboardText() const
{
    Clipboard clipboard;
    QByteArray qba = clipboard.value("text/plain");
    return QString::fromUtf8( qba.data() );
}


QByteArray Persistance::convertToUtf8(QString const& text) {
	return text.toUtf8();
}


void Persistance::donate() {
    openUri("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=dar.as.sahaba@hotmail.com&currency_code=CAD&no_shipping=1&tax=0&lc=CA&bn=PP-DonationsBF&item_name=Da'wah Activities, Rent and Utility Expenses for the Musalla (please do not use credit cards)");
}


void Persistance::forceSync()
{
    m_flags.sync();
    m_settings.sync();
}


QVariant Persistance::getValueFor(QString const& objectName)
{
    QVariant value = m_pending.contains(objectName) ? m_pending.value(objectName) : m_settings.value(objectName);

    if ( !m_logMap.contains(objectName) )
    {
        m_logMap.insert(objectName, true);

#if defined(QT_NO_DEBUG)
        int type = value.type();

        if ( type < QMetaType::Double || (type == QMetaType::QChar) || (type >= QMetaType::QString && type <= QMetaType::QStringList) ) { // no need to log that info already found in settings file in release mode
            return value;
        }
#endif

        LOGGER(objectName << value);
    }

    return value;
}


bool Persistance::contains(QString const& key) const {
	return m_pending.contains(key) || m_settings.contains(key);
}


bool Persistance::saveValueFor(QString const& key, QVariant const& value, bool fireEvent)
{
    bool isPending = m_pending.contains(key);

    if ( isPending && ( m_pending.value(key) == value ) ) {
        return false;
    }

	if ( m_settings.value(key) != value )
	{
	    LOGGER(key << value);

        m_settings.setValue(key, value);
        m_logMap.remove(key);

	    if (fireEvent) {
	        m_settings.setValue(key, value);
	        emit settingChanged(key);

	        QObjectList l = m_settingToListeners.value(key);

	        foreach (QObject* q, l) {
	            QMetaObject::invokeMethod( q, "onSettingChanged", Qt::QueuedConnection, Q_ARG(QVariant, value), Q_ARG(QVariant, key) );
	        }
	    }

	    if (isPending) {
	        m_pending[key] = value;
	    }

		return true;
	} else {
		return false;
	}
}


void Persistance::remove(QString const& key, bool fireEvent)
{
    if ( m_pending.contains(key) ) {
        m_pending.remove(key);
    }

	if ( contains(key) )
	{
	    LOGGER(key);
		m_settings.remove(key);
		m_logMap.remove(key);

		if (fireEvent) {
	        emit settingChanged(key);
		}
	}
}


bool Persistance::containsFlag(QString const& key) {
    return m_flags.contains(key);
}


QVariant Persistance::getFlag(QString const& key) {
    return m_flags.value(key);
}


void Persistance::setFlag(QString const& key, QVariant const& value)
{
    if ( value.isNull() ) {
        m_flags.remove(key);
    } else {
        m_flags.setValue(key, value);

        QObjectList l = m_settingToListeners.value(key);

        foreach (QObject* q, l) {
            QMetaObject::invokeMethod( q, "onSettingChanged", Qt::QueuedConnection, Q_ARG(QVariant, value), Q_ARG(QVariant, key) );
        }
    }
}


void Persistance::openUri(QString const& uri) {
    invoke("sys.browser", "bb.action.OPEN", "", uri);
}


void Persistance::launchSettingsApp(QString const& key, QVariantMap const& metadata) {
    InvocationUtils::launchSettingsApp(key, metadata, m_invokeManager);
}


void Persistance::reviewApp() {
    invoke("sys.appworld.review", "bb.action.OPEN", "text/html", "appworld://review");
}


void Persistance::openBlackBerryWorld(QString const& appID) {
    invoke("sys.appworld", "bb.action.OPEN", "text/html", "appworld://content/"+appID);
}


void Persistance::openChannel() {
    invoke("sys.bbm.channels.card.previewer", "bb.action.OPENBBMCHANNEL", "", "bbmc:C0034D28B");
}


void Persistance::clear()
{
    m_flags.clear();
	m_settings.clear();
}


void Persistance::launchAppPermissionSettings()
{
    QVariantMap qvm;
    qvm["appId"] = bb::PackageInfo().installId();

    launchSettingsApp("permissions", qvm);
}


bool Persistance::hasLocationAccess() {
    return InvocationUtils::hasLocationAccess();
}


bool Persistance::hasSharedFolderAccess() {
    return InvocationUtils::hasSharedFolderAccess();
}


bool Persistance::hasEmailSmsAccess() {
    return InvocationUtils::hasEmailSmsAccess();
}


bool Persistance::hasPhoneControlAccess() {
    return InvocationUtils::hasPhoneControlAccess();
}


void Persistance::portLegacy(QStringList settingKeys)
{
    QMap<QString, QVariant> interested;

    foreach (QString const& key, settingKeys)
    {
        QVariant v = m_settings.value(key);

        if ( v.isValid() ) {
            interested[key] = v;
        }
    }

    clear();
    forceSync();

    settingKeys = interested.keys();

    foreach (QString const& key, settingKeys) {
        m_settings.setValue( key, interested.value(key) );
    }
}


void Persistance::invoke(QString const& targetId, QString const& action, QString const& mime, QString const& uri, QString const& data, QObject* callback)
{
    InvokeTargetReply* itr = InvocationUtils::invoke(targetId, action, mime, uri, data, m_invokeManager);
    itr->setProperty(INVOKE_TARGET_PROPERTY, targetId);

    if (callback)
    {
        itr->setParent(callback);
        connect( itr, SIGNAL( finished() ), this, SLOT( onInvokeFinished() ) );
    }
}


void Persistance::onInvokeFinished()
{
    InvokeTargetReply* itr = static_cast<InvokeTargetReply*>( sender() );
    InvokeReplyError::Type ire = itr->error();

    LOGGER("InvokeFinished" << ire);

    QString target = itr->property(INVOKE_TARGET_PROPERTY).toString();
    QObject* caller = itr->parent();
    QMetaObject::invokeMethod( caller, INVOKE_CALLBACK_FUNC, Qt::QueuedConnection, Q_ARG(QVariant, target), Q_ARG(QVariant, ire == InvokeReplyError::None) );

    itr->deleteLater();
}


bool Persistance::isBlocked() const {
    return m_dialogs.isBlocked();
}


bool Persistance::isUpdateNeeded(QString const& key, int diffDaysMin)
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime lastUpdateCheck = QDateTime::fromMSecsSinceEpoch( getFlag(key).toLongLong() );
    int diff = lastUpdateCheck.daysTo(now);

    return diff > diffDaysMin;
}


void Persistance::call(QString const& number) {
    InvocationUtils::call(number, m_invokeManager);
}


InvokeManager* Persistance::invokeManager() {
    return m_invokeManager;
}


QString Persistance::homePath() {
    return QDir::homePath();
}


QString Persistance::tempPath() {
    return QDir::tempPath();
}


Persistance::~Persistance()
{
}


} /* namespace canadainc */
