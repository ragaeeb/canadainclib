#include "InvocationUtils.h"
#include "Logger.h"

#include <bb/system/SystemToast>

namespace {

void showBlockingToast(QString const& text, QString const& buttonLabel, QString const& icon)
{
    bb::system::SystemToast toast;
    toast.button()->setLabel(buttonLabel);
    toast.setBody(text);
    toast.setIcon(icon);
    toast.exec();
}

}

namespace canadainc {

InvocationUtils::InvocationUtils(QObject* parent) : QObject(parent)
{
}


InvocationUtils::~InvocationUtils()
{
}


void InvocationUtils::launchSettingsApp(QString const& key)
{
	bb::system::InvokeRequest request;
	request.setTarget("sys.settings.target");
	request.setAction("bb.action.OPEN");
	request.setMimeType("settings/view");
	request.setUri( QUrl("settings://"+key) );

	bb::system::InvokeManager().invoke(request);
}


void InvocationUtils::replyToMessage(qint64 accountId, QString const& messageId, InvokeManager& invokeManager)
{
	LOGGER(accountId << messageId);

	bb::system::InvokeRequest request;
	request.setAction("bb.action.REPLY");
	request.setMimeType("message/rfc822");
	request.setUri( QString("pim:message/rfc822:%1:%2").arg(accountId).arg(messageId) );

	invokeManager.invoke(request);
}


void InvocationUtils::launchLocationServices() {
	launchSettingsApp("location");
}


void InvocationUtils::launchBBM()
{
	bb::system::InvokeRequest request;
	request.setTarget("sys.bbm");
	request.setAction("bb.action.OPEN");

	bb::system::InvokeManager().invoke(request);
}


void InvocationUtils::launchSMSComposer(QString const& number, InvokeManager& invokeManager)
{
	LOGGER(number);

	bb::system::InvokeRequest request;
	request.setTarget("sys.pim.text_messaging.composer");
	request.setAction("bb.action.SENDTEXT");
	request.setUri( QString("tel:%1").arg(number) );
	invokeManager.invoke(request);
}


void InvocationUtils::launchBBMCall(QString const& pin, bool videoEnabled)
{
	LOGGER(pin << videoEnabled);

	bb::system::InvokeRequest request;
	request.setTarget("sys.service.videochat");
	request.setAction("bb.action.OPEN");
	request.setPerimeter(bb::system::SecurityPerimeter::Personal);
	request.setData( QString("dest=%1&video=%2").arg(pin).arg( videoEnabled ? 1 : 0 ).toUtf8() );
	bb::system::InvokeManager().invoke(request);
}


void InvocationUtils::launchBBMChat(QString const& pin, InvokeManager& invokeManager)
{
	LOGGER(pin);

	bb::system::InvokeRequest request;
	request.setTarget("sys.bbm.sharehandler");
	request.setAction("bb.action.BBMCHAT");
	request.setUri( QString("pin:%1").arg(pin) );
	invokeManager.invoke(request);
}


void InvocationUtils::launchEmailComposer(QString const& address, InvokeManager& invokeManager)
{
	LOGGER(address);

	bb::system::InvokeRequest request;
	request.setTarget("sys.pim.uib.email.hybridcomposer");
	request.setAction("bb.action.OPEN, bb.action.SENDEMAIL");
	request.setUri( QString("mailto:%1").arg(address) );
	invokeManager.invoke(request);
}


void InvocationUtils::launchPhoto(QString const& uri, InvokeManager* invokeManager)
{
	LOGGER(uri);

	bb::system::InvokeRequest request;
	request.setTarget("sys.pictures.card.previewer");
	request.setAction("bb.action.VIEW");
	request.setUri( QString("file://%1").arg(uri) );
	invokeManager->invoke(request);
}


void InvocationUtils::launchDoc(QString const& uri, InvokeManager& invokeManager)
{
	LOGGER(uri);

	int periodIndex = uri.lastIndexOf(".");

	if (periodIndex != -1)
	{
		QString extension = uri.mid(periodIndex);

		QString target;
		QString mimeType;

		if (extension == "ppt" || extension == "pot" || extension == "pps" || extension == "pptx" || extension == "potx" || extension == "ppsx" || extension == "pptm" || extension == "potm" || extension == "ppsm") {
			//request.insert("mime", "application/vnd.ms-powerpoint");
			target = "sys.slideshowtogo.previewer";
		} else if (extension == "xls" || extension == "xlt" || extension == "xlsx" || extension == "xltx" || extension == "xlsm" || extension == "xltm") {
			//mimeType = "application/vnd.ms-excel, application/vnd.openxmlformats-officedocument.spreadsheetml.sheet, application/vnd.openxmlformats-officedocument.spreadsheetml.template, application/vnd.ms-excel.sheet.macroEnabled.12, application/vnd.ms-excel.template.macroEnabled.12";
			target = "sys.sheettogo.previewer";
		} else if (extension == "doc" || extension == "dot" || extension == "txt" || extension == "docx" || extension == "dotx" || extension == "docm" || extension == "dotm") {
			//mimeType = "application/msword";
			target = "sys.wordtogo.previewer";
		} else if (extension == "pdf") {
			target = "com.rim.bb.app.adobeReader.viewer";
		}

		bb::system::InvokeRequest request;
		request.setTarget(target);
		request.setAction("bb.action.VIEW");
		request.setUri( QString("file://%1").arg(uri) );
		invokeManager.invoke(request);
	}
}


void InvocationUtils::launchBrowser(QString const& uri)
{
    bb::system::InvokeManager invoker;

    bb::system::InvokeRequest request;
    request.setTarget("sys.browser");
    request.setAction("bb.action.OPEN");
    request.setUri(uri);

    invoker.invoke(request);
}


void InvocationUtils::launchAudio(QString const& uri)
{
	bb::system::InvokeManager invokeManager;

	bb::system::InvokeRequest request;
	request.setTarget("sys.mediaplayer.previewer");
	request.setAction("bb.action.OPEN");
	request.setMimeType("audio/m4a");
	request.setUri( QString("file://%1").arg(uri) );

	invokeManager.invoke(request);
}


} /* namespace canadainc */
