#include "PimUtil.h"
#include "Logger.h"
#include "InvocationUtils.h"

#include <bb/pim/contacts/ContactService>

#include <bb/pim/message/MessageBuilder>
#include <bb/pim/message/MessageService>

#include <bb/PpsObject>

#include <bb/system/InvokeManager>

#include <bb/data/JsonDataAccess>

namespace canadainc {

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


qint64 PimUtil::sendMessage(MessageService* ms, Message const& m, QString text, QList<Attachment> const& attachments, bool replyPrefix)
{
    QString ck = m.conversationId();
    qint64 accountKey = m.accountId();
    LOGGER( m.sender().address() << ck << text << accountKey);

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
    LOGGER("USING ACCOUNT KEY" << accountKey );
    MessageKey mk = ms->send(accountKey, reply);

    LOGGER("Sent, now deleting messagebuilder" << mk );

    delete mb;

    return mk;
}


qint64 PimUtil::extractIdsFromInvoke(QString const& uri, QByteArray const& data, qint64& accountId)
{
    qint64 messageId = 0;

    if ( !data.isEmpty() )
    {
        bb::data::JsonDataAccess jda;
        QVariantMap json = jda.loadFromBuffer(data).toMap().value("attributes").toMap();

        if ( json.contains("accountid") && json.contains("messageid") )
        {
            accountId = json.value("accountid").toLongLong();
            messageId = json.value("messageid").toLongLong();
        }
    } else if ( !uri.isEmpty() ) {
        QStringList tokens = uri.split(":");

        if ( tokens.size() > 3 ) {
            accountId = tokens[2].toLongLong();
            messageId = tokens[3].toLongLong();
        } else {
            LOGGER("NotEnoughTokens" << tokens);
        }
    }

    LOGGER("Tokens" << accountId << messageId);

    return messageId;
}


} /* namespace canadainc */
