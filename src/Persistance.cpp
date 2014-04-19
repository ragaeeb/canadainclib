#include <bb/system/Clipboard>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>

#include <bb/cascades/QmlDocument>

#include "Persistance.h"
#include "Logger.h"

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
		showToast( tr("Copied: %1 to clipboard").arg(text) );
	}
}


void Persistance::finished(bb::system::SystemUiResult::Type value) {
	emit toastFinished(value == SystemUiResult::ButtonSelection);
}


QByteArray Persistance::convertToUtf8(QString const& text) {
	return text.toUtf8();
}


QVariant Persistance::getValueFor(const QString &objectName)
{
    QVariant value( m_settings.value(objectName) );

    LOGGER("getValueFor: " << objectName << value);

    return value;
}


bool Persistance::contains(QString const& key) {
	return m_settings.contains(key);
}


bool Persistance::saveValueFor(const QString &objectName, const QVariant &inputValue)
{
	LOGGER("saveValueFor: " << objectName << inputValue);

	if ( m_settings.value(objectName) != inputValue ) {
		m_settings.setValue(objectName, inputValue);
		emit settingChanged(objectName);
		return true;
	} else {
		LOGGER("Duplicate value, ignoring");
		return false;
	}
}


void Persistance::remove(QString const& key)
{
	LOGGER("remove key: " << key);

	if ( contains(key) ) {
		LOGGER("contains key: " << key);
		m_settings.remove(key);
		LOGGER("removed");
		emit settingChanged(key);
	}
}


void Persistance::clear() {
	m_settings.clear();
}


} /* namespace canadainc */
