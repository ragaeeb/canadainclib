#ifndef ANALYTICHELPER_H_
#define ANALYTICHELPER_H_

#include <QObject>
#include <QMap>
#include <QPair>

#define ANALYTIC_DB_NAME "analytics"

namespace canadainc {

class CustomSqlDataSource;

class AnalyticHelper : public QObject
{
    Q_OBJECT

    QMap< QPair<QString, QString>, int> m_counters;
    bool m_enabled;

private slots:
    void onAboutToQuit();
    void onDataLoaded(int id, QVariant const& data);

public:
    AnalyticHelper();
    virtual ~AnalyticHelper();

    Q_SLOT void commitStats();
    void record(QString const& event, QString const& context, bool force=false);
    void reset();
    void setEnabled(bool enabled);
};

} /* namespace canadainc */

#endif /* ANALYTICHELPER_H_ */
