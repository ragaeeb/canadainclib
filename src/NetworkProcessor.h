#ifndef NETWORKPROCESSOR_H_
#define NETWORKPROCESSOR_H_

#include <QHash>
#include <QQueue>
#include <QVariantMap>

#include <QNetworkConfigurationManager>

class QNetworkAccessManager;
class QNetworkReply;

namespace canadainc {

class NetworkProcessor : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool online READ online NOTIFY onlineChanged)

	QHash<QString, QString> m_headers;
    QNetworkAccessManager* m_networkManager;
    QQueue<QNetworkReply*> m_currentRequests;
    QNetworkConfigurationManager m_config;

    void init();

signals:
	void downloadProgress(QVariant const& cookie, qint64 bytesReceived, qint64 bytesTotal);
	void requestComplete(QVariant const& cookie, QByteArray const& data);
	void onlineChanged();

private slots:
	void onNetworkReply(QNetworkReply* reply);
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

public:
	NetworkProcessor(QObject* parent=NULL);
	virtual ~NetworkProcessor();

	Q_INVOKABLE void doRequest(QString const& uri, QVariant const& cookie=QVariant(), QVariantMap const& parameters=QVariantMap());
    Q_INVOKABLE void doGet(QString const& uri, QVariant const& cookie=QVariant());
    Q_INVOKABLE void abort();
	void setHeaders(QHash<QString,QString> const& headers);
	bool online() const;
};

} /* namespace canadainc */
#endif /* NETWORKPROCESSOR_H_ */
