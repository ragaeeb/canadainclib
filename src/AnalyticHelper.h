#ifndef ANALYTICHELPER_H_
#define ANALYTICHELPER_H_

#include <QObject>

#define ANALYTICS_PATH QString("%1/analytics.db").arg( QDir::homePath() )

namespace canadainc {

class CustomSqlDataSource;

enum ReportType
{
    AppVersionDiff,
    BugReportAuto,
    BugReportManual,
    FirstInstall,
    OsVersionDiff,
    Periodic,
    Simulation
};

struct Report
{
    QStringList attachments;
    QByteArray data;
    bool dumpAll;
    qint64 id;
    QString md5;
    QString notes;
    ReportType type;

    Report(ReportType t, bool dumpAllSlog) : dumpAll(dumpAllSlog), id( QDateTime::currentMSecsSinceEpoch() ), type(t)
    {
    }
};

class AnalyticHelper : public QObject
{
    Q_OBJECT

    QMap< QPair<QString, QString>, int> m_counters;

    CustomSqlDataSource* initAnalytics();

private slots:
    bool commitStats(bool termination=false);
    void onAboutToQuit();

public:
    AnalyticHelper();
    virtual ~AnalyticHelper();

    void clear();
    void record(QString const& event, QString const& context);
};

} /* namespace canadainc */

#endif /* ANALYTICHELPER_H_ */
