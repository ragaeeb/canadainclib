#ifndef APPLOGFETCHER_H_
#define APPLOGFETCHER_H_

#include <QFutureWatcher>

#include "NetworkProcessor.h"

#define DEVICE_INFO_LOG QString("%1/deviceInfo.txt").arg( QDir::tempPath() )
#define START_LOGGING_KEY "startLogging"
#define STOP_LOGGING_KEY "stopLogging"
#define UI_KEY "logUI"
#define UI_LOG_FILE QString("%1/logs/ui.log").arg( QDir::currentPath() )
#define ZIP_FILE_PATH QString("%1/logs.zip").arg( QDir::tempPath() )

namespace canadainc {

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

    NetworkProcessor m_network;
    QFutureWatcher<QByteArray> m_future;
    LogCollector* m_collector;

    void cleanUp();

private slots:
    void onFinished();
    void onRequestComplete(QVariant const& cookie, QByteArray const& data);
    void onReplyError();
    void startCollection();

signals:
    void submitted(QString const& message);
    void progress(QVariant const& cookie, qint64 bytesSent, qint64 bytesTotal);

public:
    AppLogFetcher(LogCollector* collector, QObject* parent=NULL);
    virtual ~AppLogFetcher();

    static void dumpDeviceInfo();
    void submitLogsLegacy();
    Q_INVOKABLE void submitLogs(QString const& notes=QString());
};

} /* namespace canadainc */

#endif /* APPLOGFETCHER_H_ */
