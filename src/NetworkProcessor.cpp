#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>

#include "NetworkProcessor.h"
#include "Logger.h"

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
    reply->setProperty("cookie", cookie);

    m_currentRequests << reply;
}


void NetworkProcessor::init()
{
	if (m_networkManager == NULL) {
		m_networkManager = new QNetworkAccessManager(this);
	}
}


void NetworkProcessor::doGet(QString const& uri, QVariant const& cookie)
{
	LOGGER( uri.right( uri.lastIndexOf("/") ) << cookie );

	init();

    QNetworkReply* reply = m_networkManager->get( QNetworkRequest( QUrl(uri) ) );
    connect( reply, SIGNAL( downloadProgress(qint64,qint64) ), this, SLOT( downloadProgress(qint64,qint64) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( onNetworkReply() ) );
    reply->setProperty("cookie", cookie);

    m_currentRequests << reply;
}


void NetworkProcessor::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	QVariant cookie = sender()->property("cookie");
	emit downloadProgress(cookie, bytesReceived, bytesTotal);
}


void NetworkProcessor::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QVariant cookie = sender()->property("cookie");
    emit uploadProgress(cookie, bytesSent, bytesTotal);
}


void NetworkProcessor::onNetworkReply()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>( sender() );

    QVariant cookie = reply->property("cookie");
    QByteArray result;

	if ( reply->error() == QNetworkReply::NoError )
	{
		if ( reply->isReadable() )
		{
			LOGGER("ReplyReadable");
			result = reply->readAll();
		} else {
			LOGGER("Unreadable!");
		}
	} else {
		LOGGER("ReplyError!" << reply->errorString() << reply->error() );
	}

	m_currentRequests.removeAll(reply);
	reply->deleteLater();

    emit requestComplete(cookie, result);
}


void NetworkProcessor::upload(QString const& uri, QString const& name, QByteArray const& contents, QVariant const& cookie)
{
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

    QUrl url = QUrl(uri);
    QNetworkRequest request(url); // DON'T TRY TO OPTIMIZE THIS BY MERGING WITH ABOVE LINE, IT DOESN'T WORK!
    request.setHeader(QNetworkRequest::ContentTypeHeader, tr("multipart/form-data; boundary=") + bound);

    QNetworkReply* reply = m_networkManager->post( request, dataToSend );
    reply->setProperty("cookie", cookie);

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
		current->abort();
		current->deleteLater();
	}
}


void NetworkProcessor::getFileSize(QString const& uri, QVariant const& cookie)
{
    init();

    QNetworkReply* reply = m_networkManager->head( QNetworkRequest( QUrl(uri) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( onSizeFetched() ) );

    reply->setProperty("cookie", cookie);
}


void NetworkProcessor::onSizeFetched()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>( sender() );
    qint64 result = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    QVariant cookie = sender()->property("cookie");

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
