#ifndef APPLOGFETCHER_H_
#define APPLOGFETCHER_H_

#include <QFutureWatcher>
#include <QSet>
#include <QSettings>
#include <QStringList>

#include "AnalyticHelper.h"
#include "NetworkProcessor.h"
#include "Report.h"

#define KEY_REPORT_NOTES "notes"

namespace canadainc {

class Persistance;

class AppLogFetcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAdmin READ adminEnabled CONSTANT FINAL)
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged FINAL)

    static AppLogFetcher* instance;
    NetworkProcessor m_network;
    QFutureWatcher<Report> m_future;
    Persistance* m_settings;
    CompressFiles m_compressor;
    bool m_dumpAll;
    AnalyticHelper m_analytics;
    QQueue<Report> m_queue; // we use this queue in case multiple different BugReportAutos are generated in short bursts

    AppLogFetcher(Persistance* settings, CompressFiles func, QObject* parent=NULL, bool dumpAll=true);

private slots:
    void onFinished();
    void onRequestComplete(QVariant const& cookie, QByteArray const& data, bool error);
    void securityCheck();

signals:
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
    void disableAnalytics();
    Q_SLOT int performCII(int analyticDiffDays=30);
    Q_INVOKABLE bool deferredCheck(QString const& key, qint64 diff, bool versionBased=false);
    Q_INVOKABLE void record(QString const& event, QString const& context="");
    static void onErrorMessage(const char* msg);
    Q_SLOT void onError(QString const& error);

    Q_INVOKABLE void submitAttributes(QVariantMap const& attributes);
    Q_INVOKABLE void submitReport(QString const& name, QString const& email, QString const& notes, QStringList const& attachments);
    void submitReport(Report r);
};

} /* namespace canadainc */

#endif /* APPLOGFETCHER_H_ */
