#include "Report.h"

#include <bb/data/JsonDataAccess>

namespace canadainc {

AppLaunch::AppLaunch(QString const& name, int t, qreal created, qreal posted, qreal visible) :
        appName(name), launchType(t), processCreated(created),
        windowPosted(posted), fullyVisible(visible)
{
}

Report::Report(ReportType::Type t) :
        dumpAll(false), type(t)
{
}

Report::Report() :
        dumpAll(false), type(ReportType::BugReportManual)
{
}

} /* namespace canadainc */
