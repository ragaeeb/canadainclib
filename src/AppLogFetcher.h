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
#define SERVICE_LOG_FILE QString("%1/logs/service.log").arg( QDir::currentPath() )
#define UI_LOG_FILE QString("%1/logs/ui.log").arg( QDir::currentPath() )
#define ZIP_FILE_PATH QString("%1/logs.zip").arg( QDir::tempPath() )

namespace canadainc {

using namespace bb::system;

class LogCollector
{
public:
    LogCollector() {};
    virtual QString appName() const { return QString(); };
    virtual QByteArray compressFiles() { return QByteArray(); };
    ~LogCollector() {};
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

public:
    AppLogFetcher(LogCollector* collector, QObject* parent=NULL);
    virtual ~AppLogFetcher();

    static void dumpDeviceInfo();
    static void enableLogging(bool enable);
    void submitLogsLegacy();
    Q_INVOKABLE void submitLogs(bool silent=false);
};

} /* namespace canadainc */

#endif /* APPLOGFETCHER_H_ */
