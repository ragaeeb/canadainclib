#ifndef NETWORKPROCESSOR_H_
#define NETWORKPROCESSOR_H_

#include <QHash>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QVariantMap>

class QNetworkAccessManager;
class QNetworkReply;

namespace canadainc {

class NetworkProcessor : public QObject
{
	Q_OBJECT

	QHash<QString, QString> m_headers;
    QNetworkAccessManager* m_networkManager;
    QQueue<QNetworkReply*> m_currentRequests;

    void init();

signals:
	void downloadProgress(QVariant const& cookie, qint64 bytesReceived, qint64 bytesTotal);
	void requestComplete(QVariant const& cookie, QByteArray const& data);

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
};

} /* namespace canadainc */
#endif /* NETWORKPROCESSOR_H_ */
