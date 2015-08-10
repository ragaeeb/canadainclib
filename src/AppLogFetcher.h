#ifndef APPLOGFETCHER_H_
#define APPLOGFETCHER_H_

#include <QFutureWatcher>
#include <QSet>
#include <QStringList>

#include "AdminUtils.h"
#include "AnalyticHelper.h"
#include "NetworkProcessor.h"
#include "Report.h"

namespace canadainc {

class Persistance;

class AppLogFetcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAdmin READ adminEnabled NOTIFY adminEnabledChanged)
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)

    static AppLogFetcher* instance;
    AdminUtils m_admin;
    NetworkProcessor m_network;
    QFutureWatcher<Report> m_future;
    Persistance* m_settings;
    CompressFiles m_compressor;
    bool m_dumpAll;
    AnalyticHelper m_analytics;

    AppLogFetcher(Persistance* settings, CompressFiles func, QObject* parent=NULL, bool dumpAll=true);

private slots:
    void onAuthenticate(QUrl const& q);
    void onFinished();
    void onRequestComplete(QVariant const& cookie, QByteArray const& data, bool error);
    void securityCheck();

signals:
    void adminEnabledChanged();
    void onlineChanged();
    void progress(QVariant const& cookie, qint64 bytesSent, qint64 bytesTotal);
    void submitted(QString const& message);
    void latestAppVersionFound(QString const& version);

public:
    static AppLogFetcher* create(Persistance* settings, CompressFiles func, QObject* parent=NULL, bool dumpAll=true);
    static AppLogFetcher* getInstance();
    virtual ~AppLogFetcher();

    bool adminEnabled() const;
    bool online() const;
    Q_SLOT bool performCII();
    Q_INVOKABLE void checkForUpdate(QString const& projectName);
    Q_INVOKABLE bool deferredCheck(QString const& key, qint64 diff, bool versionBased=false);
    Q_INVOKABLE void record(QString const& event, QString const& context="");
    static void onErrorMessage(const char* msg);
    Q_SLOT void onError(QString const& error);

    Q_INVOKABLE void submitReport(QString const& name, QString const& email, QString const& notes, QStringList const& attachments);
    void submitReport(Report r);
};

} /* namespace canadainc */

#endif /* APPLOGFETCHER_H_ */
