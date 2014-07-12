#ifndef APPLOGFETCHER_H_
#define APPLOGFETCHER_H_

#include <QFutureWatcher>

#include "NetworkProcessor.h"

#define START_LOGGING_KEY "startLogging"
#define STOP_LOGGING_KEY "stopLogging"
#define UI_KEY "logUI"
#define REMOVED_APPS_PATH QString("%1/removedapps").arg( QDir::tempPath() )
#define DEVICE_INFO_PATH QString("%1/deviceInfo.txt").arg( QDir::tempPath() )
#define UI_LOG_FILE QString("%1/logs/ui.log").arg( QDir::currentPath() )
#define DEFAULT_LOGS QStringList() << QSettings().fileName() << DEVICE_INFO_PATH << UI_LOG_FILE << REMOVED_APPS_PATH << "/var/boottime.txt"
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

    static AppLogFetcher* instance;
    NetworkProcessor m_network;
    QFutureWatcher<QByteArray> m_future;
    LogCollector* m_collector;

    AppLogFetcher(LogCollector* collector, QObject* parent=NULL);
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
    static void create(LogCollector* collector, QObject* parent=NULL);
    static AppLogFetcher* getInstance();
    virtual ~AppLogFetcher();

    static void dumpDeviceInfo(QString const& additional=QString());
    void submitLogsLegacy();
    Q_INVOKABLE void submitLogs(QString const& notes=QString());
};

} /* namespace canadainc */

#endif /* APPLOGFETCHER_H_ */
