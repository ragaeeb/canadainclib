#include "PimUtil.h"
#include "InvocationUtils.h"
#include "Logger.h"
#include "Persistance.h"

#include <bb/pim/calendar/CalendarService>
#include <bb/pim/calendar/CalendarSettings>

#include <bb/pim/contacts/ContactService>

#include <bb/pim/message/MessageBuilder>
#include <bb/pim/message/MessageService>

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
	map.insert( "accountid", ACCOUNT_KEY_SMS );
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


bool PimUtil::validateEmailSMSAccess(QString const& message, bool launchAppPermissions)
{
    if ( !InvocationUtils::hasEmailSmsAccess()  )
    {
        LOGGER("messages.db did not exist!");
        Persistance::showBlockingToast( message, tr("OK"), "asset:///images/ic_pim_warning.png" );

        if (launchAppPermissions) {
            InvocationUtils::launchAppPermissionSettings();
        }

        return false;
    }

    return true;
}


qint64 PimUtil::sendMessage(MessageService* ms, Message const& m, QString text, QList<Attachment> const& attachments, bool replyPrefix)
{
    QString ck = m.conversationId();
    qint64 accountKey = m.accountId();
    LOGGER("==========" << m.sender().address() << ck << text << accountKey);

    const MessageContact from = m.sender();

    MessageBuilder* mb = MessageBuilder::create(accountKey);
    mb->conversationId(ck);

    if (accountKey != ACCOUNT_KEY_SMS) {
        LOGGER("ADDING BODY TEXT" << text);
        const MessageContact mc = MessageContact( from.id(), MessageContact::To, from.name(), from.address() );
        mb->addRecipient(mc);
        mb->subject( replyPrefix ? tr("RE: %1").arg( m.subject() ) : m.subject() );
        mb->body( MessageBody::Html, text.replace("\n", "<br>").toUtf8() );
    } else {
        mb->addRecipient(from);

        LOGGER("ADDING ATTACHMENT TEXT" << text);
        mb->addAttachment( Attachment("text/plain", "<primary_text.txt>", text) );
    }

    for (int i = attachments.size()-1; i >= 0; i--) {
        mb->addAttachment( attachments[i] );
    }

    LOGGER("Replying with" << m.sender().displayableName() << ck << text);

    Message reply = *mb;
    LOGGER("======== USING ACCOUNT KEY" << accountKey );
    MessageKey mk = ms->send(accountKey, reply);

    LOGGER("Sent, now deleting messagebuilder" << mk );

    delete mb;

    return mk;
}

} /* namespace canadainc */
