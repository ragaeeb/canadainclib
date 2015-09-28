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

private slots:
    void onAboutToQuit();
    void onDataLoaded(int id, QVariant const& data);

public:
    AnalyticHelper();
    virtual ~AnalyticHelper();

    void reset();
    Q_SLOT void commitStats();
    void record(QString const& event, QString const& context);
};

} /* namespace canadainc */

#endif /* ANALYTICHELPER_H_ */
