#ifndef NETWORKPROCESSOR_H_
#define NETWORKPROCESSOR_H_

#include <QHash>
#include <QQueue>
#include <QVariantMap>

#include <QNetworkConfigurationManager>

#define HTTP_RESPONSE_OK "200 OK"

class QNetworkAccessManager;
class QNetworkReply;

namespace canadainc {

class NetworkProcessor : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool online READ online NOTIFY onlineChanged FINAL)

	QHash<QString, QString> m_headers;
    QNetworkAccessManager* m_networkManager;
    QQueue<QNetworkReply*> m_currentRequests;
    QNetworkConfigurationManager m_config;
    bool m_aborted;

    void init();

signals:
	void downloadProgress(QVariant const& cookie, qint64 bytesReceived, qint64 bytesTotal);
    void onlineChanged();
	void requestComplete(QVariant const& cookie, QByteArray const& data, bool error);
	void sizeFetched(QVariant const& cookie, qint64 size);
    void uploadProgress(QVariant const& cookie, qint64 bytesSent, qint64 bytesTotal);

private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void onNetworkReply();
    void onlineStateChanged(bool online);
    void onSizeFetched();
	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

public:
	NetworkProcessor(QObject* parent=NULL);
	virtual ~NetworkProcessor();

	Q_INVOKABLE void doRequest(QString const& uri, QVariant const& cookie=QVariant(), QVariantMap const& parameters=QVariantMap());
    Q_INVOKABLE void doGet(QUrl const& uri, QVariant const& cookie=QVariant());
    Q_INVOKABLE void upload(QUrl const& uri, QString const& name, QByteArray const& qba, QVariant const& cookie=QVariant());
    Q_INVOKABLE void getFileSize(QString const& uri, QVariant const& cookie);
    Q_SLOT void abort();
	void setHeaders(QHash<QString,QString> const& headers);
	bool online() const;
};

} /* namespace canadainc */
#endif /* NETWORKPROCESSOR_H_ */
