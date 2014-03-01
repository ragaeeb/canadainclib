#include "customsqldatasource.h"
#include "IOUtils.h"
#include "Logger.h"

#include <bb/data/SqlConnection>
#include <QFile>
#include <QDateTime>

namespace canadainc {

using namespace bb::data;

CustomSqlDataSource::CustomSqlDataSource(QObject *parent) : QObject(parent), m_name("connect"), m_sqlConnector(NULL), m_execTimestamp(0)
{
}

CustomSqlDataSource::~CustomSqlDataSource()
{
	if (m_sqlConnector) {
		m_sqlConnector->stop();
	}
}


void CustomSqlDataSource::setQuery(QString const& query)
{
    if (m_query.compare(query) != 0) {
        m_query = query;
        emit queryChanged(m_query);
    }
}

QString CustomSqlDataSource::query() const {
    return m_query;
}

void CustomSqlDataSource::setName(QString const& name)
{
    if (m_name.compare(name) != 0) {
    	m_name = name;
        emit nameChanged(m_name);
    }
}

QString CustomSqlDataSource::name() const {
    return m_name;
}

bool CustomSqlDataSource::checkConnection()
{
    if (m_sqlConnector) {
        return true;
    } else {
        QFile newFile(m_source);

        if (newFile.exists()) {

            // Remove the old connection if it exists
            if(m_sqlConnector){
                disconnect( m_sqlConnector, SIGNAL( reply(bb::data::DataAccessReply const&) ), this, SLOT( onLoadAsyncResultData(bb::data::DataAccessReply const&) ) );
                m_sqlConnector->setParent(NULL);
                delete m_sqlConnector;
            }

            // Set up a connection to the data base
            m_sqlConnector = new SqlConnection(m_source, m_name, this);

            // Connect to the reply function
            connect(m_sqlConnector, SIGNAL( reply(bb::data::DataAccessReply const&) ), this, SLOT( onLoadAsyncResultData(bb::data::DataAccessReply const&) ) );

            return true;

        } else {
            LOGGER("Failed to load data base, file does not exist.");
            emit error( "Failed to load data base, file does not exist: "+m_source+". If this is in your SD Card, make sure your USB Mass Storage Mode is set to Off." );
        }
    }

    return false;
}

DataAccessReply CustomSqlDataSource::executeAndWait(QVariant const& criteria, int id)
{
    DataAccessReply reply;

    if ( checkConnection() )
    {
        reply = m_sqlConnector->executeAndWait(criteria, id);

        if ( reply.hasError() ) {
            LOGGER("error " << reply);
        }
    }

    return reply;
}

void CustomSqlDataSource::execute(QVariant const& criteria, int id)
{
    if ( checkConnection() ) {
    	LOGGER("Starting query" << id << criteria);
    	m_execTimestamp = QDateTime::currentMSecsSinceEpoch();
        m_sqlConnector->execute(criteria, id);
    }
}


void CustomSqlDataSource::executePrepared(QVariantList const& values, int id)
{
    if ( checkConnection() ) {
    	LOGGER("Starting query" << m_query << values << id);
    	m_execTimestamp = QDateTime::currentMSecsSinceEpoch();
        m_sqlConnector->execute(m_query, values, id);
    }
}


void CustomSqlDataSource::executeTransaction(QStringList const& queries, int id)
{
	if ( checkConnection() )
	{
    	LOGGER("Starting query" << queries << id);
    	m_execTimestamp = QDateTime::currentMSecsSinceEpoch();
		m_sqlConnector->beginTransaction(id);

		for (int i = 0; i < queries.size(); i++) {
			m_sqlConnector->execute( queries[i], id );
		}

		m_sqlConnector->endTransaction(id);
	}
}


void CustomSqlDataSource::executePreparedTransaction(QStringList const& queries, QVariantList const& values, int id)
{
	if ( checkConnection() && queries.size() == values.size() )
	{
    	LOGGER("Starting query" << queries << values << id);
    	m_execTimestamp = QDateTime::currentMSecsSinceEpoch();
		m_sqlConnector->beginTransaction(id);

		for (int i = 0; i < queries.size(); i++) {
			m_sqlConnector->execute(queries[i], values[i].toList(), id);
		}

		m_sqlConnector->endTransaction(id);
	}
}


void CustomSqlDataSource::load(int id)
{
    if ( !m_query.isEmpty() ) {
    	LOGGER("query is not empty..." << m_query);
        execute(m_query, id);
    }
}

void CustomSqlDataSource::onLoadAsyncResultData(bb::data::DataAccessReply const& replyData)
{
	LOGGER("Query " << replyData.id() << "took " << QDateTime::currentMSecsSinceEpoch()-m_execTimestamp );

    if ( replyData.hasError() ) {
        LOGGER( replyData.id() << ", SQL error: " << replyData );
    } else {
        QVariantList resultList = replyData.result().toList();
        LOGGER( "Result list" << resultList.size() << resultList );
        emit dataLoaded( replyData.id(), resultList );
    }
}


bool CustomSqlDataSource::initSetup(QStringList const& setupStatements, int id)
{
	bool result = IOUtils::writeFile(m_source);
	executeTransaction(setupStatements, id);

	return result;
}


void CustomSqlDataSource::setSource(QString const& source) {
	m_source = source;
}


QString CustomSqlDataSource::source() const {
	return m_source;
}

}
