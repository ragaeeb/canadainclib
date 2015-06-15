#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>

#include "NetworkProcessor.h"
#include "Logger.h"

#define KEY_COOKIE "cookie"

namespace canadainc {

NetworkProcessor::NetworkProcessor(QObject* parent) :
        QObject(parent), m_networkManager(NULL)
{
    connect( &m_config, SIGNAL( onlineStateChanged(bool) ), this, SLOT( onlineStateChanged(bool) ) );
}


void NetworkProcessor::onlineStateChanged(bool online)
{
    Q_UNUSED(online);
    emit onlineChanged();
}


void NetworkProcessor::doRequest(QString const& uri, QVariant const& cookie, QVariantMap const& parameters)
{
    if ( uri.isEmpty() )
    {
        LOGGER("EmptyURIGiven");
        return;
    }

	LOGGER( uri.right( uri.lastIndexOf("/") ) << parameters );

    QUrl params;

    QStringList keys = m_headers.keys();

    foreach (QString key, keys) {
    	params.addQueryItem( key, m_headers[key] );
    }

    keys = parameters.keys();

    foreach (QString key, keys) {
    	params.addQueryItem( key, parameters[key].toString() );
    }

    QByteArray data;
    data.append( params.toString() );
    data.remove(0,1);

	init();

	QNetworkRequest qnr(uri);
	qnr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = m_networkManager->post(qnr, data);
    connect( reply, SIGNAL( downloadProgress(qint64,qint64) ), this, SLOT( downloadProgress(qint64,qint64) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( onNetworkReply() ) );
    reply->setProperty(KEY_COOKIE, cookie);

    m_currentRequests << reply;
}


void NetworkProcessor::init()
{
	if (m_networkManager == NULL) {
		m_networkManager = new QNetworkAccessManager(this);
	}
}


void NetworkProcessor::doGet(QUrl const& uri, QVariant const& cookie)
{
    if ( uri.isEmpty() )
    {
        LOGGER("EmptyURIGiven");
        return;
    }

	LOGGER( uri.toString(QUrl::RemoveUserInfo | QUrl::RemovePassword) );

	init();

    QNetworkReply* reply = m_networkManager->get( QNetworkRequest(uri) );
    connect( reply, SIGNAL( downloadProgress(qint64,qint64) ), this, SLOT( downloadProgress(qint64,qint64) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( onNetworkReply() ) );
    reply->setProperty(KEY_COOKIE, cookie);

    m_currentRequests << reply;
}


void NetworkProcessor::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	QVariant cookie = sender()->property(KEY_COOKIE);
	emit downloadProgress(cookie, bytesReceived, bytesTotal);
}


void NetworkProcessor::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QVariant cookie = sender()->property(KEY_COOKIE);
    emit uploadProgress(cookie, bytesSent, bytesTotal);
}


void NetworkProcessor::onNetworkReply()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>( sender() );

    QVariant cookie = reply->property(KEY_COOKIE);
    QByteArray result;
    bool error = true;

	if ( reply->error() == QNetworkReply::NoError )
	{
		if ( reply->isReadable() )
		{
			LOGGER("ReplyReadable");
			result = reply->readAll();
			error = false;
		} else {
			LOGGER("Unreadable!");
		}
	} else {
		LOGGER("ReplyError!" << reply->errorString() << reply->error() );
	}

	m_currentRequests.removeAll(reply);
	reply->deleteLater();

    emit requestComplete(cookie, result, error);
}


void NetworkProcessor::upload(QUrl const& url, QString const& name, QByteArray const& contents, QVariant const& cookie)
{
    if ( url.isEmpty() )
    {
        LOGGER("EmptyURIGiven");
        return;
    }

    LOGGER( url.toString(QUrl::RemoveUserInfo | QUrl::RemovePassword) );

    init();

    QString bound;
    QString crlf;
    QString data;
    QByteArray dataToSend;
    bound = "---------------------------7d935033608e2";
    crlf = 0x0d;
    crlf += 0x0a;
    data = "--" + bound + crlf + "Content-Disposition: form-data; name=\"archiveFile\"; ";
    data += QString("filename=\"%1\"").arg(name);
    data += crlf + "Content-Type: application/octet-stream" + crlf + crlf;
    dataToSend.insert(0, data);
    dataToSend.append(contents);
    dataToSend.append(crlf + "--" + bound + "--" + crlf);

    QNetworkRequest request(url); // DON'T TRY TO OPTIMIZE THIS BY MERGING WITH ABOVE LINE, IT DOESN'T WORK!
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=%1").arg(bound));

    QNetworkReply* reply = m_networkManager->post( request, dataToSend );
    reply->setProperty(KEY_COOKIE, cookie);

    connect( reply, SIGNAL( uploadProgress(qint64,qint64) ), this, SLOT( uploadProgress(qint64,qint64) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( onNetworkReply() ) );

    m_currentRequests << reply;
}


void NetworkProcessor::setHeaders(QHash<QString,QString> const& headers) {
	m_headers = headers;
}


void NetworkProcessor::abort()
{
	while ( !m_currentRequests.isEmpty() )
	{
		QNetworkReply* current = m_currentRequests.dequeue();
	    disconnect( current, SIGNAL( downloadProgress(qint64,qint64) ), this, SLOT( downloadProgress(qint64,qint64) ) );
	    disconnect( current, SIGNAL( finished() ), this, SLOT( onNetworkReply() ) );
	    //current->disconnect();
	    //QObject::disconnect(current, 0, 0, 0);
		current->abort();
		current->deleteLater();
	}
}


void NetworkProcessor::getFileSize(QString const& uri, QVariant const& cookie)
{
    init();

    QNetworkReply* reply = m_networkManager->head( QNetworkRequest( QUrl(uri) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( onSizeFetched() ) );

    reply->setProperty(KEY_COOKIE, cookie);
}


void NetworkProcessor::onSizeFetched()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>( sender() );
    qint64 result = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    QVariant cookie = sender()->property(KEY_COOKIE);

    LOGGER(cookie << result);

    sender()->deleteLater();

    emit sizeFetched(cookie, result);
}


bool NetworkProcessor::online() const {
    return m_config.isOnline();
}


NetworkProcessor::~NetworkProcessor()
{
	abort();
}

} /* namespace canadainc */
