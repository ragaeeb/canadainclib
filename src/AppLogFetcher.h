#ifndef APPLOGFETCHER_H_
#define APPLOGFETCHER_H_

#include <QFutureWatcher>
#include <QSet>
#include <QStringList>

#include "NetworkProcessor.h"

#define REMOVED_APPS_PATH QString("%1/removedapps.txt").arg( QDir::tempPath() )
#define DEVICE_INFO_PATH QString("%1/deviceInfo.txt").arg( QDir::tempPath() )
#define SLOG2_PATH QString("%1/slog2.txt").arg( QDir::tempPath() )
#define NOTES_PATH QString("%1/notes.txt").arg( QDir::tempPath() )
#define PIDIN_PATH QString("%1/pidin.txt").arg( QDir::tempPath() )
#define PROGRESS_MANAGER_PATH QString("%1/progress_manager.txt").arg( QDir::tempPath() )
#define DEFAULT_LOGS QStringList() << QSettings().fileName() << DEVICE_INFO_PATH << NOTES_PATH << PROGRESS_MANAGER_PATH << SLOG2_PATH << REMOVED_APPS_PATH << "/var/boottime.txt" << "/var/app_launch_data.txt"
#define ZIP_FILE_PATH QString("%1/logs.zip").arg( QDir::tempPath() )

namespace bb {
    namespace cascades {
        class KeyEvent;
    }
}

namespace canadainc {

class Persistance;

typedef void (*CompressFiles)(QSet<QString>&);

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
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)

    static AppLogFetcher* instance;
    NetworkProcessor m_network;
    QFutureWatcher< QPair<QByteArray,QString> > m_future;
    AdminData m_admin;
    Persistance* m_settings;
    CompressFiles m_compressor;

    AppLogFetcher(Persistance* settings, CompressFiles func, QObject* parent=NULL);

private slots:
    void onFinished();
    void onKeyReleasedHandler(bb::cascades::KeyEvent* event);
    void onRequestComplete(QVariant const& cookie, QByteArray const& data, bool error);

signals:
    void adminEnabledChanged();
    void onlineChanged();
    void progress(QVariant const& cookie, qint64 bytesSent, qint64 bytesTotal);
    void simulationFilesAvailable(QStringList const& files);
    void submitted(QString const& message);
    void latestAppVersionFound(QString const& version);

public:
    static AppLogFetcher* create(Persistance* settings, CompressFiles func, QObject* parent=NULL);
    static AppLogFetcher* getInstance();
    virtual ~AppLogFetcher();

    bool adminEnabled() const;
    bool online() const;
    Q_SLOT bool performCII();
    Q_INVOKABLE void checkForUpdate(QString const& projectName);
    Q_INVOKABLE void initPage(QObject* page);
    Q_INVOKABLE void submitLogs(QString const& notes=QString(), bool userTriggered=false, bool includeLastScreenshot=false, bool isSimulation=false);
    Q_INVOKABLE void previewLastCapturedPic();
    static void removeInvalid(QSet<QString>& input);
};

} /* namespace canadainc */

#endif /* APPLOGFETCHER_H_ */
