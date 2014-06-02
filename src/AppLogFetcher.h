#ifndef APPLOGFETCHER_H_
#define APPLOGFETCHER_H_

#include <QMap>
#include <QFutureWatcher>

class QNetworkAccessManager;
class QNetworkReply;

namespace bb {
    namespace system {
        class SystemProgressToast;
    }
}

#define DEVICE_INFO_LOG QString("%1/deviceInfo.txt").arg( QDir::tempPath() )
#define START_LOGGING_KEY "startLogging"
#define STOP_LOGGING_KEY "stopLogging"
#define UI_KEY "logUI"
#define UI_LOG_FILE QString("%1/logs/ui.log").arg( QDir::currentPath() )
#define ZIP_FILE_PATH QString("%1/logs.zip").arg( QDir::tempPath() )

namespace canadainc {

using namespace bb::system;

class LogCollector
{
public:
    LogCollector();
    virtual QString appName() const = 0;
    virtual QByteArray compressFiles() = 0;
    virtual ~LogCollector();
};

class AppLogFetcher : public QObject
{
    Q_OBJECT

    QNetworkAccessManager* m_networkManager;
    SystemProgressToast* m_progress;
    QFutureWatcher<QByteArray> m_future;
    LogCollector* m_collector;

private slots:
    void onFinished();
    void onNetworkReply(QNetworkReply* reply);
    void startCollection();
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

signals:
    void submitted(QString const& message);

public:
    AppLogFetcher(LogCollector* collector, QObject* parent=NULL);
    virtual ~AppLogFetcher();

    static void dumpDeviceInfo();
    void submitLogsLegacy();
    Q_INVOKABLE void submitLogs(bool silent=false, QString const& notes=QString());
};

} /* namespace canadainc */

#endif /* APPLOGFETCHER_H_ */
