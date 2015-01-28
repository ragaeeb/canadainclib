#ifndef APPLOGFETCHER_H_
#define APPLOGFETCHER_H_

#include <QFutureWatcher>
#include <QStringList>

#include "NetworkProcessor.h"

#define START_LOGGING_KEY "startLogging"
#define STOP_LOGGING_KEY "stopLogging"
#define UI_KEY "logUI"
#define REMOVED_APPS_PATH QString("%1/removedapps.txt").arg( QDir::tempPath() )
#define DEVICE_INFO_PATH QString("%1/deviceInfo.txt").arg( QDir::tempPath() )
#define SLOG2_PATH QString("%1/slog2.txt").arg( QDir::tempPath() )
#define NOTES_PATH QString("%1/notes.txt").arg( QDir::tempPath() )
#define PIDIN_PATH QString("%1/pidin.txt").arg( QDir::tempPath() )
#define UI_LOG_FILE QString("%1/logs/ui.log").arg( QDir::currentPath() )
#define DEFAULT_LOGS QStringList() << QSettings().fileName() << DEVICE_INFO_PATH << NOTES_PATH << UI_LOG_FILE << SLOG2_PATH << REMOVED_APPS_PATH << "/var/boottime.txt" << "/var/app_launch_data.txt"
#define ZIP_FILE_PATH QString("%1/logs.zip").arg( QDir::tempPath() )

namespace bb {
    namespace cascades {
        class KeyEvent;
    }
}

namespace canadainc {

class Persistance;

class LogCollector
{
public:
    LogCollector();
    virtual QByteArray compressFiles() = 0;
    virtual ~LogCollector();
};

struct AdminData
{
    QStringList buffer;
    QString expected;
    bool isAdmin;
};

class AppLogFetcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAdmin READ adminEnabled NOTIFY adminEnabledChanged)

    static AppLogFetcher* instance;
    NetworkProcessor m_network;
    QFutureWatcher<QByteArray> m_future;
    LogCollector* m_collector;
    AdminData m_admin;
    Persistance* m_settings;

    AppLogFetcher(Persistance* settings, LogCollector* collector, QObject* parent=NULL);
    void cleanUp();

private slots:
    void onFinished();
    void onKeyReleasedHandler(bb::cascades::KeyEvent* event);
    void onRequestComplete(QVariant const& cookie, QByteArray const& data);
    void onReplyError();
    void startCollection();

signals:
    void adminEnabledChanged();
    void progress(QVariant const& cookie, qint64 bytesSent, qint64 bytesTotal);
    void submitted(QString const& message);
    void latestAppVersionFound(QString const& version);

public:
    static AppLogFetcher* create(Persistance* settings, LogCollector* collector, QObject* parent=NULL);
    static AppLogFetcher* getInstance();
    virtual ~AppLogFetcher();

    static void dumpDeviceInfo(QString const& additional=QString());
    bool adminEnabled() const;
    Q_SLOT bool performCII();
    Q_INVOKABLE void checkForUpdate(QString const& projectName);
    Q_INVOKABLE void initPage(QObject* page);
    Q_INVOKABLE void submitLogs(QString const& notes=QString(), bool userTriggered=false);
    void submitLogsLegacy();
};

} /* namespace canadainc */

#endif /* APPLOGFETCHER_H_ */
