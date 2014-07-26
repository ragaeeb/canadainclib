#include <bb/system/Clipboard>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>

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
        Q_FOREACH( QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst) )
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
}

Persistance::~Persistance()
{
    /*
    QStringList keys = m_pending.keys();

    foreach ( QString const& key, m_pending.keys() ) {
        m_settings.setValue( key, m_pending.value(key) );
    } */
}


void Persistance::showToast(QString const& text, QString const& buttonLabel, QString const& icon)
{
	if (m_toast == NULL) {
		m_toast = new SystemToast(this);
		connect( m_toast, SIGNAL( finished(bb::system::SystemUiResult::Type) ), this, SLOT( finished(bb::system::SystemUiResult::Type) ) );
	}

	m_toast->button()->setLabel(buttonLabel);
	m_toast->setBody(text);
	m_toast->setIcon(icon);
	m_toast->show();
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
	SystemDialog dialog;
	dialog.setBody(text);
	dialog.setTitle(title);
	dialog.confirmButton()->setLabel(okButton);
	dialog.cancelButton()->setLabel(cancelButton);

	return dialog.exec() == SystemUiResult::ConfirmButtonSelection;
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


void Persistance::finished(bb::system::SystemUiResult::Type value) {
	emit toastFinished(value == SystemUiResult::ButtonSelection);
}


QByteArray Persistance::convertToUtf8(QString const& text) {
	return text.toUtf8();
}


QVariant Persistance::getValueFor(const QString &objectName) const
{
    QVariant value = /*m_pending.contains(objectName) ? m_pending.value(objectName) : */m_settings.value(objectName);
    LOGGER(objectName << value);

    return value;
}


bool Persistance::contains(QString const& key) {
	return m_pending.contains(key) || m_settings.contains(key);
}


bool Persistance::saveValueFor(const QString &objectName, const QVariant &inputValue, bool fireEvent)
{
    Q_UNUSED(fireEvent);

	if ( m_settings.value(objectName) != inputValue )
	{
	    LOGGER(objectName << inputValue);

        m_settings.setValue(objectName, inputValue);
        emit settingChanged(objectName);

	    /*
	    if (fireEvent) {
	        m_settings.setValue(objectName, inputValue);
	        emit settingChanged(objectName);
	    } else {
	        m_pending[objectName] = inputValue;
	    } */

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

		if (fireEvent) {
	        emit settingChanged(key);
		}
	}
}


bool Persistance::tutorial(QString const& key, QString const& message, QString const& icon)
{
    if ( !contains(key) )
    {
        showToast( message, tr("OK"), icon );
        saveValueFor(key, 1, false);

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


void Persistance::reviewApp(bool prompt, QString const& key, QString const& message)
{
    if (prompt)
    {
        if ( contains(key) ) {
            return;
        }

        prompt = showBlockingDialog( tr("Review"), message, tr("Yes"), tr("No") );

        if (!prompt) {
            return;
        }

        saveValueFor(key, 1, false);
    }

    InvokeRequest request;
    request.setTarget("sys.appworld.review");
    request.setAction("bb.action.OPEN");
    request.setMimeType("text/html");
    request.setUri("appworld://review");

    InvokeManager().invoke(request);
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


void Persistance::clearCache()
{
    bool clear = showBlockingDialog( tr("Confirmation"), tr("Are you sure you want to clear the cache?"), tr("Yes"), tr("No") );

    if (clear) {
        QFutureWatcher<void>* qfw = new QFutureWatcher<void>(this);
        connect( qfw, SIGNAL( finished() ), this, SLOT( cacheCleared() ) );

        QFuture<void> future = QtConcurrent::run(clearAllCache);
        qfw->setFuture(future);
    }
}


void Persistance::cacheCleared() {
    showToast( tr("Cache was successfully cleared!"), "", "file:///usr/share/icons/bb_action_delete.png" );
}


void Persistance::clear() {
	m_settings.clear();
}


} /* namespace canadainc */
