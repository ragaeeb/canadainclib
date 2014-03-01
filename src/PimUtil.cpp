#include "PimUtil.h"
#include "InvocationUtils.h"
#include "Logger.h"
#include "MessageManager.h"
#include "Persistance.h"

#include <bb/pim/message/Message>

#include <bb/pim/calendar/CalendarService>
#include <bb/pim/calendar/CalendarSettings>

#include <bb/pim/contacts/ContactService>

#include <bb/PpsObject>

#include <bb/system/InvokeManager>

namespace canadainc {

void PimUtil::openEmail(qint64 accountId, qint64 messageId)
{
	LOGGER("==========" << accountId << messageId);
	bb::system::InvokeManager invokeManager;

	bb::system::InvokeRequest request;
	request.setAction("bb.action.VIEW");
	request.setMimeType("message/rfc822");
	request.setUri( QString("pim:message/rfc822:%1:%2").arg(accountId).arg(messageId) );

	invokeManager.invoke(request);
}


void PimUtil::openSMSMessage(QString const& conversationKey, qint64 messageId)
{
	LOGGER("==========" << conversationKey << messageId);
	bb::system::InvokeManager invokeManager;

	QVariantMap map;
	map.insert( "accountid", MessageManager::account_key_sms );
	map.insert( "messageid", messageId );
	map.insert( "conversationid", conversationKey );

	QByteArray requestData = bb::PpsObject::encode(map, NULL);

	bb::system::InvokeRequest request;
	request.setTarget("sys.pim.text_messaging");
	request.setAction("bb.action.VIEW");
	request.setMimeType("application/text_messaging");
	request.setData(requestData);

	invokeManager.invoke(request);
}


void PimUtil::launchPhoneCall(QString const& number)
{
	QVariantMap map;
	map.insert("number", number);
	QByteArray requestData = bb::PpsObject::encode(map, NULL);

	bb::system::InvokeRequest request;
	request.setAction("bb.action.DIAL");
	request.setMimeType("application/vnd.blackberry.phone.startcall");
	request.setData(requestData);

	InvokeManager().invoke(request);
}


void PimUtil::replyToSMS(QString const& senderAddress, QString const& body, InvokeManager& invokeManager)
{
	LOGGER(senderAddress << body);

	bb::system::InvokeRequest request;
	request.setTarget("sys.pim.text_messaging.composer");
	request.setAction("bb.action.COMPOSE");
	request.setMimeType("application/text_messaging");

	QVariantMap data;
	data["to"] = QVariantList() << senderAddress;
	data["send"] = false;
	data["body"] = body;
	request.setData( bb::PpsObject::encode(data) );

	invokeManager.invoke(request);
}


QString PimUtil::extractText(Message const& m)
{
	QString text = m.body(MessageBody::PlainText).plainText();

	if ( text.isEmpty() ) {
		text = m.body(MessageBody::Html).plainText();
	}

	if ( text.isEmpty() && m.attachmentCount() > 0 && m.attachmentAt(0).mimeType() == "text/plain" ) {
		text = QString::fromLocal8Bit( m.attachmentAt(0).data() );
	}

	return text;
}


bool PimUtil::validateCalendarAccess(QString const& message, bool launchAppPermissions)
{
	bb::pim::calendar::CalendarSettings cs = bb::pim::calendar::CalendarService().settings();

	if ( !cs.isValid() )
	{
		Persistance::showBlockingToast( message, tr("OK") );

		if (launchAppPermissions) {
			InvocationUtils::launchAppPermissionSettings();
		}

		return false;
	}

	return true;
}


bool PimUtil::validateContactsAccess(QString const& message, bool launchAppPermissions)
{
	bb::pim::contacts::ContactService cs;
	int count = cs.count( bb::pim::contacts::ContactListFilters() );

	if (count == 0) {
		Persistance::showBlockingToast( message, tr("OK") );

		if (launchAppPermissions) {
			InvocationUtils::launchAppPermissionSettings();
		}
	}

	return count > 0;
}

} /* namespace canadainc */
