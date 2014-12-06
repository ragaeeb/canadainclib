#include <bb/system/Clipboard>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>

#include "Persistance.h"
#include "InvocationUtils.h"
#include "Logger.h"

namespace {

bool removeDir(QString const& dirName)
{
    bool result = true;
    QDir dir(dirName);

    if ( dir.exists(dirName) )
    {
        foreach( QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst) )
        {
            if ( info.isDir() ) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove( info.absoluteFilePath() );
            }
        }

        result = dir.rmdir(dirName);
    }

    return result;
}

void clearAllCache()
{
    QString homePath = QDir::homePath();

    QFile::remove( QString("%1/WebpageIcons.db").arg(homePath) );
    QFile::remove( QString("%1/cookieCollection.db").arg(homePath) );
    QFile::remove( QString("%1/cookieCollection.db-wal").arg(homePath) );
    QFile::remove( QString("%1/storagequota.db").arg(homePath) );

    removeDir( QString("%1/appcache").arg(homePath) );
    removeDir( QString("%1/cache").arg(homePath) );
    removeDir( QString("%1/certificates").arg(homePath) );
    removeDir( QString("%1/downloads").arg(homePath) );
    removeDir( QString("%1/localstorage").arg(homePath) );
}

}

namespace canadainc {

using namespace bb::cascades;
using namespace bb::system;

Persistance::Persistance(QObject* parent) : QObject(parent), m_toast(NULL)
{
    QDeclarativeContext* rootContext = QmlDocument::defaultDeclarativeEngine()->rootContext();
    rootContext->setContextProperty("persist", this);

    connect( Application::instance(), SIGNAL( aboutToQuit() ), this, SLOT( commit() ) );
}


void Persistance::commit()
{
    QStringList keys = m_pending.keys();

    foreach ( QString const& key, m_pending.keys() ) {
        m_settings.setValue( key, m_pending.value(key) );
    }
}


void Persistance::showToast(QString const& text, QString const& buttonLabel, QString const& icon, bb::system::SystemUiPosition::Type pos)
{
	if (m_toast == NULL) {
		m_toast = new SystemToast(this);
        connect( m_toast, SIGNAL( finished(bb::system::SystemUiResult::Type) ), this, SLOT( finished(bb::system::SystemUiResult::Type) ) );
	}

	m_toast->button()->setLabel(buttonLabel);
	m_toast->setBody(text);
	m_toast->setIcon(icon);
	m_toast->setProperty("showing", true);
	m_toast->setPosition(pos);
	m_toast->show();
}


void Persistance::finished(bb::system::SystemUiResult::Type value)
{
    Q_UNUSED(value);
    sender()->setProperty("showing", false);
}


bool Persistance::showBlockingToast(QString const& text, QString const& buttonLabel, QString const& icon)
{
	SystemToast toast;
	toast.button()->setLabel(buttonLabel);
	toast.setBody(text);
	toast.setIcon(icon);

	return toast.exec() == SystemUiResult::ButtonSelection;
}


bool Persistance::showBlockingDialog(QString const& title, QString const& text, QString const& okButton, QString const& cancelButton)
{
	bool remember = false;
	return showBlockingDialog(title, text, QString(), remember, okButton, cancelButton);
}


bool Persistance::showBlockingDialog(QString const& title, QString const& text, QString const& rememberMeText, bool &rememberMeValue, QString const& okButton, QString const& cancelButton)
{
    SystemDialog dialog;
    dialog.setBody(text);
    dialog.setTitle(title);
    dialog.confirmButton()->setLabel(okButton);
    dialog.cancelButton()->setLabel(cancelButton);

    bool showRememberMe = !rememberMeText.isNull();

    if (showRememberMe)
    {
        dialog.setIncludeRememberMe(true);
        dialog.setRememberMeChecked(rememberMeValue);
        dialog.setRememberMeText(rememberMeText);
    }

    bool result = dialog.exec() == SystemUiResult::ConfirmButtonSelection;
    rememberMeValue = dialog.rememberMeSelection();

    return result;
}


void Persistance::copyToClipboard(QString const& text, bool showToastMessage)
{
	Clipboard clipboard;
	clipboard.clear();
	clipboard.insert( "text/plain", convertToUtf8(text) );

	if (showToastMessage) {
		showToast( tr("Copied: %1 to clipboard").arg(text), "", "asset:///images/menu/ic_copy.png" );
	}
}


QByteArray Persistance::convertToUtf8(QString const& text) {
	return text.toUtf8();
}


void Persistance::forceSync() {
    m_settings.sync();
}


QVariant Persistance::getValueFor(QString const& objectName)
{
    QVariant value = m_settings.value(objectName);

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


bool Persistance::saveValueFor(const QString &objectName, const QVariant &inputValue, bool fireEvent)
{
	if ( m_settings.value(objectName) != inputValue )
	{
	    LOGGER(objectName << inputValue);

        m_settings.setValue(objectName, inputValue);
        m_logMap.remove(objectName);

	    if (fireEvent) {
	        m_settings.setValue(objectName, inputValue);
	        emit settingChanged(objectName);
	    } else {
	        m_pending[objectName] = inputValue;
	    }

		return true;
	} else {
		return false;
	}
}


void Persistance::remove(QString const& key, bool fireEvent)
{
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


bool Persistance::tutorial(QString const& key, QString const& message, QString const& icon)
{
    if ( !contains(key) )
    {
        if ( !m_toast || !m_toast->property("showing").toBool() )
        {
            showToast( message, tr("OK"), icon, SystemUiPosition::BottomCenter );
            saveValueFor(key, 1, false);
        }

        return true;
    }

    return false;
}


bool Persistance::tutorialVideo(QString const& uri, bool prompt, QString const& key, QString const& message)
{
    if (!prompt) {
        InvocationUtils::launchBrowser(uri);
        return true;
    }

    if ( !contains(key) )
    {
        bool result = showBlockingDialog( tr("Tutorial"), message, tr("Yes"), tr("No") );

        if (result) {
            InvocationUtils::launchBrowser(uri);
        }

        saveValueFor(key, 1, false);
    }

    return false;
}


void Persistance::donate(QString const& uri) {
    InvocationUtils::launchBrowser(uri);
}


void Persistance::reviewApp()
{
    InvokeRequest request;
    request.setTarget("sys.appworld.review");
    request.setAction("bb.action.OPEN");
    request.setMimeType("text/html");
    request.setUri("appworld://review");

    InvokeManager().invoke(request);
}


bool Persistance::reviewed()
{
    if ( !contains("alreadyReviewed") )
    {
        bool yes = showBlockingDialog( tr("Review"), tr("If you enjoy the app, we would really appreciate if you left us a review so we can improve! It should only take a second. Would you like to leave one?"), tr("Yes"), tr("No") );
        saveValueFor("alreadyReviewed", 1, false);

        if (yes) {
            reviewApp();
        }

        return true;
    }

    return false;
}


void Persistance::openChannel(bool promote)
{
    if (promote)
    {
        if ( contains("promoted") ) {
            return;
        }

        saveValueFor("promoted", 1, false);
    }

    InvokeRequest request;
    request.setTarget("sys.bbm.channels.card.previewer");
    request.setAction("bb.action.OPENBBMCHANNEL");
    request.setUri("bbmc:C0034D28B");

    InvokeManager().invoke(request);
}


bool Persistance::clearCache()
{
    bool clear = showBlockingDialog( tr("Confirmation"), tr("Are you sure you want to clear the cache?") );

    if (clear) {
        QFutureWatcher<void>* qfw = new QFutureWatcher<void>(this);
        connect( qfw, SIGNAL( finished() ), this, SLOT( cacheCleared() ) );

        QFuture<void> future = QtConcurrent::run(clearAllCache);
        qfw->setFuture(future);
    }

    return clear;
}


void Persistance::cacheCleared() {
    showToast( tr("Cache was successfully cleared!"), "", "file:///usr/share/icons/bb_action_delete.png" );
}


void Persistance::clear() {
	m_settings.clear();
}


void Persistance::launchAppPermissionSettings() {
    InvocationUtils::launchAppPermissionSettings();
}


bool Persistance::validateLocationAccess(QString const& message, bool launchAppPermissions)
{
    QFile target("/pps/services/geolocation/control");

    if ( !target.open(QIODevice::ReadOnly) )
    {
        showBlockingToast( message, tr("OK"), "file:///usr/share/icons/ic_map_all.png" );

        if (launchAppPermissions) {
            launchAppPermissionSettings();
        }

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


Persistance::~Persistance()
{
}


} /* namespace canadainc */
