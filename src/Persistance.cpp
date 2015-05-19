#include <bb/system/Clipboard>
#include <bb/system/SystemDialog>
#include <bb/system/SystemPrompt>
#include <bb/system/SystemToast>

#include <bb/cascades/QmlDocument>

#include <bb/PackageInfo>
#include <bb/PpsObject>

#include <sys/utsname.h>

#include <QCoreApplication>

#include "Persistance.h"
#include "InvocationUtils.h"
#include "IOUtils.h"
#include "Logger.h"

#include "bbndk.h"

#if BBNDK_VERSION_AT_LEAST(10,3,1)
#include <bb/cascades/Control>
#include <bb/cascades/DeviceShortcut>
#endif

#define KEY_TOAST_SHOWING "showing"
#define KEY_ARGS "args"
#define KEY_CALLBACK "callback"
#define METHOD_NAME "onFinished"

namespace {

bool isNowBlocked = false;

}

namespace canadainc {

using namespace bb::cascades;
using namespace bb::system;

Persistance::Persistance(QObject* parent) :
        QObject(parent), m_dialog(NULL), m_toast(NULL), m_prompt(NULL), m_flags(FLAGS_FILE_NAME)
{
    QDeclarativeContext* rootContext = QmlDocument::defaultDeclarativeEngine()->rootContext();
    rootContext->setContextProperty("persist", this);

    connect( QCoreApplication::instance(), SIGNAL( aboutToQuit() ), this, SLOT( commit() ) );

    isNowBlocked = false;
}


void Persistance::commit()
{
    QStringList keys = m_pending.keys();

    foreach ( QString const& key, m_pending.keys() ) {
        m_settings.setValue( key, m_pending.value(key) );
    }
}


void Persistance::showToast(QString const& text, QString const& icon, bb::system::SystemUiPosition::Type pos)
{
	if (m_toast == NULL) {
		m_toast = new SystemToast(this);
        connect( m_toast, SIGNAL( finished(bb::system::SystemUiResult::Type) ), this, SLOT( finished(bb::system::SystemUiResult::Type) ) );
	}

	m_toast->setBody(text);
	m_toast->setIcon( icon.startsWith("asset:///") || icon.startsWith("file:///") ? icon : "asset:///"+icon );
	m_toast->setProperty(KEY_TOAST_SHOWING, true);
	m_toast->setPosition(pos);
	m_toast->show();
}


void Persistance::finished(bb::system::SystemUiResult::Type value)
{
    Q_UNUSED(value);
    sender()->setProperty(KEY_TOAST_SHOWING, false);
}


bool Persistance::showBlockingDialog(QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled)
{
	bool remember = false;
	return showBlockingDialog(title, text, QString(), remember, okButton, cancelButton, okEnabled);
}


void Persistance::onDestroyed(QObject* obj)
{
    if (obj == m_dialog) {
        m_dialog = NULL;
    } else if (obj == m_prompt) {
        m_prompt = NULL;
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


void Persistance::showDialog(QString const& title, QString const& text, QString okButton) {
    showDialog(NULL, QVariant(), title, text, okButton, "");
}


void Persistance::showDialog(QObject* caller, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, QString const& rememberMeText, bool rememberMeValue) {
    showDialog(caller, QVariant(), title, text, okButton, cancelButton, true, rememberMeText, rememberMeValue);
}


void Persistance::showDialog(QObject* caller, QVariant const& data, QString const& title, QString const& text, QString const& okButton, QString const& cancelButton, bool okEnabled, QString const& rememberMeText, bool rememberMeValue)
{
    isNowBlocked = true;

    if (m_dialog == NULL)
    {
        m_dialog = new SystemDialog(caller);
        connect( m_dialog, SIGNAL( finished(bb::system::SystemUiResult::Type) ), this, SLOT( dialogFinished(bb::system::SystemUiResult::Type) ) );
        connect( m_dialog, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );
    }

    bool showRememberMe = !rememberMeText.isEmpty();
    m_dialog->setIncludeRememberMe(showRememberMe);

    if (showRememberMe)
    {
        m_dialog->setRememberMeChecked(rememberMeValue);
        m_dialog->setRememberMeText(rememberMeText);
    }

    m_dialog->setParent(caller);
    m_dialog->setBody(text);
    m_dialog->setTitle(title);
    m_dialog->cancelButton()->setLabel(cancelButton);
    m_dialog->confirmButton()->setLabel(okButton);
    m_dialog->confirmButton()->setEnabled(okEnabled);
    m_dialog->setProperty(KEY_ARGS, data);
    m_dialog->show();
}


void Persistance::dialogFinished(bb::system::SystemUiResult::Type value)
{
    isNowBlocked = false;

    QObject* caller = m_dialog->parent();

    if (caller != NULL)
    {
        bool result = value == SystemUiResult::ConfirmButtonSelection;
        QVariant data = m_dialog->property(KEY_ARGS);
        m_dialog->setParent(this);

        if ( m_dialog->includeRememberMe() )
        {
            bool rememberMe = m_dialog->rememberMeSelection();

            if ( data.isValid() ) {
                QMetaObject::invokeMethod( caller, "onFinished", Qt::QueuedConnection, Q_ARG(QVariant, result), Q_ARG(QVariant, rememberMe), Q_ARG(QVariant, data) );
            } else {
                QMetaObject::invokeMethod( caller, "onFinished", Qt::QueuedConnection, Q_ARG(QVariant, result), Q_ARG(QVariant, rememberMe) );
            }
        } else {
            if ( data.isValid() ) {
                QMetaObject::invokeMethod( caller, "onFinished", Qt::QueuedConnection, Q_ARG(QVariant, result), Q_ARG(QVariant, data) );
            } else {
                QMetaObject::invokeMethod( caller, "onFinished", Qt::QueuedConnection, Q_ARG(QVariant, result) );
            }
        }

        m_dialog->setProperty(KEY_ARGS, QVariant());
    }
}


void Persistance::showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize, QString const& okButton, QString const& cancelButton, int inputMode, QString const& funcName, QVariant const& data)
{
    isNowBlocked = true;

    SystemUiInputMode::Type m = (SystemUiInputMode::Type)inputMode;

    if (m_prompt == NULL)
    {
        m_prompt = new SystemPrompt(caller);
        connect( m_prompt, SIGNAL( finished(bb::system::SystemUiResult::Type) ), this, SLOT( promptFinished(bb::system::SystemUiResult::Type) ) );
        connect( m_prompt, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );
    }

    m_prompt->setParent(caller);
    m_prompt->setBody(body);
    m_prompt->setTitle(title);
    m_prompt->cancelButton()->setLabel(cancelButton);
    m_prompt->confirmButton()->setLabel(okButton);
    m_prompt->inputField()->setDefaultText(defaultText);
    m_prompt->inputField()->setEmptyText(hintText);
    m_prompt->inputField()->setMaximumLength(maxLength);
    m_prompt->inputField()->setInputMode(m);
    m_prompt->setInputOptions(autoCapitalize ? SystemUiInputOption::AutoCapitalize : SystemUiInputOption::None);
    m_prompt->setProperty(KEY_ARGS, data);
    m_prompt->setProperty(KEY_CALLBACK, funcName);
    m_prompt->show();
}


void Persistance::showPrompt(QObject* caller, QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, QString const& funcName, QVariant const& data) {
    showPrompt(caller, title, body, defaultText, hintText, maxLength, true, tr("Save"), tr("Cancel"), 0, funcName, data);
}


void Persistance::promptFinished(bb::system::SystemUiResult::Type value)
{
    isNowBlocked = false;

    QObject* caller = m_prompt->parent();

    if (caller != NULL)
    {
        QVariant data = m_prompt->property(KEY_ARGS);
        m_prompt->setParent(this);

        QString result = value == SystemUiResult::ConfirmButtonSelection ? m_prompt->inputFieldTextEntry().trimmed() : "";
        const char* callback = m_prompt->property(KEY_CALLBACK).toString().toUtf8().constData();

        if ( data.isValid() ) {
            QMetaObject::invokeMethod( caller, callback, Qt::QueuedConnection, Q_ARG(QVariant, result), Q_ARG(QVariant, data) );
        } else {
            QMetaObject::invokeMethod( caller, callback, Qt::QueuedConnection, Q_ARG(QVariant, result) );
        }

        m_prompt->setProperty(KEY_ARGS, QVariant());
    }
}


bool Persistance::showBlockingDialog(QString const& title, QString const& text, QString const& rememberMeText, bool &rememberMeValue, QString const& okButton, QString const& cancelButton, bool okEnabled)
{
    isNowBlocked = true;

    SystemDialog dialog;
    dialog.setBody(text);
    dialog.setTitle(title);
    dialog.confirmButton()->setLabel(okButton);
    dialog.cancelButton()->setLabel(cancelButton);
    dialog.confirmButton()->setEnabled(okEnabled);

    bool showRememberMe = !rememberMeText.isNull();

    if (showRememberMe)
    {
        dialog.setIncludeRememberMe(true);
        dialog.setRememberMeChecked(rememberMeValue);
        dialog.setRememberMeText(rememberMeText);
    }

    bool result = dialog.exec() == SystemUiResult::ConfirmButtonSelection;
    rememberMeValue = dialog.rememberMeSelection();

    isNowBlocked = false;

    return result;
}


QString Persistance::showBlockingPrompt(QString const& title, QString const& body, QString const& defaultText, QString const& hintText, int maxLength, bool autoCapitalize, QString const& okButton, QString const& cancelButton, int inputMode)
{
    isNowBlocked = true;

    SystemUiInputMode::Type m = (SystemUiInputMode::Type)inputMode;

    SystemPrompt dialog;
    dialog.setBody(body);
    dialog.setTitle(title);
    dialog.inputField()->setDefaultText(defaultText);
    dialog.inputField()->setEmptyText(hintText);
    dialog.inputField()->setMaximumLength(maxLength);
    dialog.inputField()->setInputMode(m);
    dialog.setInputOptions(autoCapitalize ? SystemUiInputOption::AutoCapitalize : SystemUiInputOption::None);
    dialog.confirmButton()->setLabel(okButton);
    dialog.cancelButton()->setLabel(cancelButton);

    bool result = dialog.exec() == SystemUiResult::ConfirmButtonSelection;

    isNowBlocked = false;

    return result ? dialog.inputFieldTextEntry() : QString();
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


void Persistance::forceSync() {
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


bool Persistance::containsFlag(QString const& key)
{
    return m_flags.contains(key);
}


QVariant Persistance::getFlag(QString const& key)
{
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


void Persistance::launchSettingsApp(QString const& key, QVariantMap const& metadata)
{
    InvokeRequest request;
    request.setTarget("sys.settings.target");
    request.setAction("bb.action.OPEN");
    request.setMimeType("settings/view");
    request.setUri( QUrl("settings://"+key) );

    if ( !metadata.isEmpty() ) {
        request.setMetadata(metadata);
    }

    m_invokeManager.invoke(request);
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


bool Persistance::clearCache()
{
    bool clear = showBlockingDialog( tr("Confirmation"), tr("Are you sure you want to clear the cache?") );

    if (clear) {
        QFutureWatcher<void>* qfw = new QFutureWatcher<void>(this);
        connect( qfw, SIGNAL( finished() ), this, SLOT( cacheCleared() ) );

        QFuture<void> future = QtConcurrent::run(&IOUtils::clearAllCache);
        qfw->setFuture(future);
    }

    return clear;
}


void Persistance::cacheCleared() {
    showToast( tr("Cache was successfully cleared!"), "file:///usr/share/icons/bb_action_delete.png" );
}


void Persistance::clear() {
	m_settings.clear();
}


void Persistance::launchAppPermissionSettings()
{
    QVariantMap qvm;
    qvm["appId"] = bb::PackageInfo().installId();

    launchSettingsApp("permissions", qvm);
}


bool Persistance::hasLocationAccess()
{
    QFile target("/pps/services/geolocation/status");

    if ( !target.open(QIODevice::ReadOnly) ) {
        return false;
    }

    target.close();

    return true;
}


bool Persistance::hasSharedFolderAccess()
{
    QString sdDirectory = QString("/accounts/1000/shared/downloads");

    if ( !QDir(sdDirectory).exists() )
    {
        LOGGER(sdDirectory << "DidNotExist!");
        return false;
    }

    return true;
}


bool Persistance::hasEmailSmsAccess()
{
    if ( QFile("/var/db/text_messaging/messages.db").exists() || QFile("/accounts/1000/_startup_data/sysdata/text_messaging/messages.db").exists() ) {
        return true;
    } else {
        struct utsname udata;
        uname(&udata);

        if ( QString(udata.machine).startsWith("x86") ) { // simulator build doesn't have it
            return true;
        }
    }

    LOGGER("NoEmailSmsAccess");
    return false;
}


bool Persistance::hasPhoneControlAccess()
{
    if ( QFile("/pps/services/phone/protected/status").exists() ) {
        return true;
    }

    LOGGER("NoPhoneControlAccess");
    return false;
}


void Persistance::attachBackKeyToClickedSignal(QObject* abstractButton, QObject* rootControl)
{
#if BBNDK_VERSION_AT_LEAST(10,3,1)
    Control* c = static_cast<Control*>(rootControl);
    DeviceShortcut* ds = DeviceShortcut::create(DeviceShortcuts::BackTap).onTriggered( abstractButton, SIGNAL( clicked() ) );
    c->addShortcut(ds);
#else
    Q_UNUSED(abstractButton);
    Q_UNUSED(rootControl);
#endif
}


void Persistance::invoke(QString const& targetId, QString const& action, QString const& mime, QString const& uri, QString const& data)
{
    InvokeRequest request;
    request.setTarget(targetId);
    request.setAction(action);
    request.setUri(uri);
    request.setMimeType(mime);
    request.setData( data.toUtf8() );

    m_invokeManager.invoke(request);
}


bool Persistance::isBlocked() const {
    return isNowBlocked;
}


bool Persistance::isUpdateNeeded(QString const& key, int diffDaysMin)
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime lastUpdateCheck = QDateTime::fromMSecsSinceEpoch( getFlag(key).toLongLong() );
    int diff = lastUpdateCheck.daysTo(now);

    LOGGER(diff);
    return diff > diffDaysMin;
}


void Persistance::call(QString const& number)
{
    LOGGER(number);
    QVariantMap map;
    map.insert("number", number);
    QByteArray requestData = bb::PpsObject::encode(map, NULL);

    bb::system::InvokeRequest request;
    request.setAction("bb.action.DIAL");
    request.setMimeType("application/vnd.blackberry.phone.startcall");
    request.setData(requestData);

    m_invokeManager.invoke(request);
}


InvokeManager* Persistance::invokeManager() {
    return &m_invokeManager;
}


Persistance::~Persistance()
{
}


} /* namespace canadainc */
