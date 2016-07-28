#ifndef REPORT_H_
#define REPORT_H_

#include <QMap>
#include <QStringList>

#define KEY_USER_ID "user_id"

namespace canadainc {

struct AppLaunch
{
    QString appName;
    int launchType;
    qreal processCreated;
    qreal windowPosted;
    qreal fullyVisible;

    AppLaunch(QString const& name, int t, qreal created, qreal posted, qreal visible);
};

struct ReportType
{
    enum Type
    {
        AppVersionDiff,
        BugReportAuto,
        BugReportManual,
        FirstInstall,
        OsVersionDiff,
        Periodic,
        Simulation,
        Attribute
    };
};

struct AddressType
{
    enum Type
    {
        Unknown,
        BBM,
        Email,
        Facebook,
        Instagram,
        Phone,
        Twitter,
        WhatsApp
    };
};

struct Address
{
    AddressType::Type type;
    QString value;

    Address(QString const& v, AddressType::Type t) : type(t), value(v) {}
};

struct Report
{
    QStringList attachments;
    QByteArray data;
    bool dumpAll;
    QString id;
    ReportType::Type type;
    QMap<QString, QString> params;
    QMap<QString, QString> removedApps;
    QList<AppLaunch> appLaunches;
    QList<Address> addresses;

    Report(ReportType::Type t);
    Report();
};

typedef void (*CompressFiles)(Report&, QString const&, const char*);

} /* namespace canadainc */

#endif /* REPORT_H_ */
