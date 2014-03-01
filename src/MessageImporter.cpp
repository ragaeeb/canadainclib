#include "MessageImporter.h"
#include "MessageManager.h"
#include "Logger.h"
#include "PimUtil.h"

#include <bb/pim/message/MessageFilter>
#include <bb/pim/message/MessageService>

namespace canadainc {

using namespace bb::pim::message;

MessageImporter::MessageImporter(qint64 accountKey, bool onlyInbound) :
		m_accountKey(accountKey), m_inboundOnly(onlyInbound),
		m_latestFirst(true), m_userAlias( tr("You") ), m_unreadOnly(false), m_deviceTime(false), m_timeLimit(INT_MAX), m_quit(false)
{
}


void MessageImporter::cancel() {
    m_quit = true;
}


QVariantList MessageImporter::processSingleConversation()
{
	MessageService messageService;
	QList<Message> messages = messageService.messagesInConversation( m_accountKey, m_conversationKey, MessageFilter() );
	QVariantList variants;

	int total = messages.size();

	if (!m_latestFirst)
	{
		for (int i = 0; i < total; i++) {
			appendIfValid(messages[i], variants);
		}
	} else {
		for (int i = total-1; i >= 0; i--) {
			appendIfValid(messages[i], variants);
		}
	}

	emit progress(total, total);

	return variants;
}


QVariantMap MessageImporter::transform(Message const& m)
{
    QVariantMap qvm;
    qvm.insert( "id", m.id() );
    qvm.insert( "inbound", m.isInbound() );
    qvm.insert( "replyTo", m.replyTo().address() );
    qvm.insert( "sender", m.sender().displayableName() );
    qvm.insert( "subject", m.subject() );
    qvm.insert( "senderAddress", m.sender().address() );
    qvm.insert( "text", canadainc::PimUtil::extractText(m) );
    qvm.insert( "time", m.deviceTimestamp() );

    if ( !m.imagePath().isEmpty() ) {
        qvm.insert( "imageSource", m.imagePath() );
    }

    return qvm;
}


void MessageImporter::appendIfValid(Message const& m, QVariantList& variants)
{
	bool validTest = !m.isDraft() && m.isValid();
	bool inboundTest = ( m.isInbound() && m_inboundOnly ) || !m_inboundOnly;
	bool readTest = ( m.status() != MessageStatus::Read && m_unreadOnly ) || !m_unreadOnly;

	if (validTest && inboundTest && readTest)
	{
	    QVariantMap qvm = transform(m);
	    qvm.insert( "sender", m.isInbound() ? m.sender().displayableName() : m_userAlias );
	    qvm.insert( "time", m_deviceTime ? m.deviceTimestamp() : m.serverTimestamp() );

		variants << transform(m);
	}
}


QVariantList MessageImporter::processAllConversations()
{
    MessageService ms;
	QList<Conversation> conversations = ms.conversations( m_accountKey, MessageFilter() );
	QVariantList result;

	int total = conversations.size();
	LOGGER("==== TOTAL" << total);

	QDateTime now = QDateTime::currentDateTime();

	if (m_accountKey == MessageManager::account_key_sms)
	{
	    LOGGER("PROCESSING SMS ROUTE");

	    for (int i = total-1; i >= 0; i--)
	    {
	        if (m_quit) {
	            return QVariantList();
	        }

	        Conversation c = conversations[i];
	        Message m = ms.message( m_accountKey, c.latestMessageId() );

	        QDateTime t = m_deviceTime ? m.deviceTimestamp() : m.serverTimestamp();

	        if ( t.daysTo(now) > m_timeLimit ) { // timelimit = 7. now-sent = 3. 3 > 7
	            break;
	        }

	        appendIfValid(m, result);
	        emit progress(i, total);
	    }
	} else {
	    LOGGER("PROCESSING EMAIL ROUTE");

	    for (int i = 0; i < total; i++)
	    {
	        if (m_quit) {
	            return QVariantList();
	        }

	        Conversation c = conversations[i];
	        Message m = ms.message( m_accountKey, c.latestMessageId() );

	        QDateTime t = m_deviceTime ? m.deviceTimestamp() : m.serverTimestamp();

	        if ( t.daysTo(now) > m_timeLimit ) { // timelimit = 7. now-sent = 3. 3 > 7
	            LOGGER("Breaking off" << t << now << t.daysTo(now) << m_timeLimit);
	            break;
	        }

	        appendIfValid(m, result);
	        emit progress(i, total);
	    }
	}

	emit progress(total, total);

	return result;
}


void MessageImporter::run()
{
	emit importCompleted( getResult() );
}


QVariantList MessageImporter::getResult()
{
	LOGGER("MessageImporter::run()" << m_accountKey << m_conversationKey);

	QVariantList result = m_conversationKey.isNull() ? processAllConversations() : processSingleConversation();
	LOGGER( "Elements generated:" << result.size() );

	return result;
}


void MessageImporter::setConversation(QString const& conversationKey) {
	m_conversationKey = conversationKey;
}


void MessageImporter::setUserAlias(QString const& alias) {
	m_userAlias = alias;
}


void MessageImporter::setLatestFirst(bool latestFirst) {
	m_latestFirst = latestFirst;
}


void MessageImporter::setUnreadOnly(bool unreadOnly) {
	m_unreadOnly = unreadOnly;
}


void MessageImporter::setUseDeviceTime(bool deviceTime) {
	m_deviceTime = deviceTime;
}


void MessageImporter::setTimeLimit(int days) {
	m_timeLimit = days;
}


MessageImporter::~MessageImporter()
{
}

} /* namespace canadainc */
