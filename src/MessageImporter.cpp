#include "MessageImporter.h"
#include "Logger.h"
#include "PimUtil.h"

#include "bbndk.h"

#include <bb/pim/message/MessageFilter>
#include <bb/pim/message/MessageService>

#if BBNDK_VERSION_AT_LEAST(10,3,0)
#include <bb/pim/phone/CallHistoryService>
#endif

using namespace bb::pim::message;

namespace {

bool latestFirst = true;
bool deviceTime = false;

bool messageComparator(Message const& c1, Message const& c2)
{
    if (deviceTime) {
        return !latestFirst ? c1.deviceTimestamp() < c2.deviceTimestamp() : c1.deviceTimestamp() > c2.deviceTimestamp();
    } else {
        return !latestFirst ? c1.serverTimestamp() < c2.serverTimestamp() : c1.serverTimestamp() > c2.serverTimestamp();
    }
}

}

namespace canadainc {

MessageImporter::MessageImporter(qint64 accountKey, bool onlyInbound) :
		m_accountKey(accountKey), m_inboundOnly(onlyInbound),
		m_latestFirst(true), m_userAlias( tr("You") ), m_unreadOnly(false),
		m_deviceTime(false), m_timeLimit(INT_MAX), m_quit(false)
{
}


void MessageImporter::cancel() {
    m_quit = true;
}


QVariantList MessageImporter::processSingleConversation()
{
    latestFirst = m_latestFirst;
    deviceTime = m_deviceTime;

	MessageService messageService;
	QList<Message> messages = messageService.messagesInConversation( m_accountKey, m_conversationKey, MessageFilter() );
	qSort( messages.begin(), messages.end(), messageComparator );
	QVariantList variants;

	int total = messages.size();

    for (int i = 0; i < total; i++) {
        appendIfValid(messages[i], variants);
    }

	emit progress(total, total);

	return variants;
}


QVariantMap MessageImporter::transform(Message const& m)
{
    QVariantMap qvm;
    qvm.insert( "id", m.id() );
    qvm.insert( "aid", m.accountId() );
    qvm.insert( "cid", m.conversationId() );
    qvm.insert( "inbound", m.isInbound() );
    qvm.insert( "replyTo", m.replyTo().address() );
    qvm.insert( "sender", m.sender().displayableName() );
    qvm.insert( "senderAddress", m.sender().address() );
    qvm.insert( "time", m.deviceTimestamp() );

    if ( !m.subject().isEmpty() ) {
        qvm.insert( "subject", m.subject() );
    }

    QString text = canadainc::PimUtil::extractText(m);

    if ( !text.isEmpty() ) {
        qvm.insert("text", text);
    }

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

		variants << qvm;
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

	if (m_accountKey == ACCOUNT_KEY_SMS)
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


QVariantList MessageImporter::fetchCalls()
{
    QVariantList result;
    int total = 0;

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    bb::pim::phone::CallHistoryService chs;
    bb::pim::phone::CallHistoryFilter chf = bb::pim::phone::CallHistoryFilter();
    bb::pim::phone::CallTypeList types;
    types << bb::pim::phone::CallType::Incoming;
    types << bb::pim::phone::CallType::Missed;
    chf.setTypeFilter(types);

    QList<bb::pim::phone::CallEntryResult> entries = chs.callHistory(8, chf);
    total = entries.size();

    LOGGER("**** Total" << total);

    for (int i = 0; i < total; i++)
    {
        bb::pim::phone::CallEntry c = entries[i].call();
        QVariantMap qvm;

        qvm["id"] = c.id();
        qvm["aid"] = c.accountId();
        qvm["senderAddress"] = c.phoneNumber();
        qvm["duration"] = c.duration();

        if ( !c.callerName().isEmpty() ) {
            qvm["sender"] = c.callerName();
        }

        qvm["time"] = c.startDate();

        result << qvm;

        emit progress(i, total);
    }
#endif

    emit progress(total, total);

    return result;
}


QVariantList MessageImporter::getResult()
{
	LOGGER("MessageImporter::run()" << m_accountKey << m_conversationKey);

	QVariantList result;

	if (m_accountKey == ACCOUNT_KEY_CELLULAR) {
	    result = fetchCalls();
	} else {
	    result = m_conversationKey.isNull() ? processAllConversations() : processSingleConversation();
	}

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
