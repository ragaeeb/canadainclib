#include "AppLogFetcher.h"
#include "Logger.h"
#include "Persistance.h"
#include "ReportGenerator.h"

#include <QtConcurrentRun>

#include <bb/Application>
#include <bb/PackageInfo>

#include <bb/system/Clipboard>

#include <bb/platform/PlatformInfo>

#include <bb/cascades/QmlDocument>

#define KEY_ADMIN_MODE "adminMode"
#define COOKIE_REPORT_TYPE "reportType"
#define DUPLICATE_GROUP "duplicate_bug"
#define DUPLICATE_KEY(hash) QString("%1/%2").arg(DUPLICATE_GROUP).arg(hash)
#define KEY_ANALYTICS_COLLECTED "analytics_collected"
#define KEY_INSTALLED_APP_VERSION "installedAppVersion"
#define KEY_LAST_OS_VERSION "lastOperatingSystem"

using namespace bb::system;

namespace {

using namespace canadainc;

void checkSigningHash()
{
    QString signingHash = bb::PackageInfo().authorSigningHash();

    if ( /*!signingHash.startsWith("test") && */signingHash != "gYAAgL4hrHsGI4tljUxgYhylQfs" ) {
        bb::Application::instance()->requestExit();
    }
}

}

namespace canadainc {

using namespace bb::cascades;

AppLogFetcher* AppLogFetcher::instance = NULL;

AppLogFetcher::AppLogFetcher(Persistance* settings, CompressFiles func, QObject* parent, bool dumpAll) :
        QObject(parent), m_settings(settings),
        m_compressor(func), m_dumpAll(dumpAll)
{
    connect( &m_future, SIGNAL( finished() ), this, SLOT( onFinished() ) );
    connect( &m_network, SIGNAL( downloadProgress(QVariant const&, qint64, qint64) ), this, SIGNAL( progress(QVariant const&, qint64, qint64) ) );
    connect( &m_network, SIGNAL( onlineChanged() ), this, SIGNAL( onlineChanged() ) );
    connect( &m_network, SIGNAL( requestComplete(QVariant const&, QByteArray const&, bool) ), this, SLOT( onRequestComplete(QVariant const&, QByteArray const&, bool) ) );
    connect( &m_network, SIGNAL( uploadProgress(QVariant const&, qint64, qint64) ), this, SIGNAL( progress(QVariant const&, qint64, qint64) ) );

    QDeclarativeContext* rootContext = QmlDocument::defaultDeclarativeEngine()->rootContext();
    rootContext->setContextProperty("reporter", this);

#if defined(QT_NO_DEBUG) && !DEBUG_RELEASE
    QTimer::singleShot( 2000, this, SLOT( securityCheck() ) );
#endif

    m_endpoint[ReportType::FirstInstall] = "analytics/get_user_id.php";
    m_endpoint[ReportType::OsVersionDiff] = "analytics/submit_os_diff.php";
    m_endpoint[ReportType::AppVersionDiff] = "analytics/submit_app_diff.php";
    m_endpoint[ReportType::Periodic] = "analytics/submit_analytics.php";
    m_endpoint[ReportType::Attribute] = "analytics/submit_attributes.php";
    m_endpoint[ReportType::BugReportAuto] = "analytics/submit_bug.php";
    m_endpoint[ReportType::BugReportManual] = "analytics/submit_bug.php";
    m_endpoint[ReportType::Custom] = "analytics/submit_bug.php";

    m_addressType[AddressType::BBM] = "bbm";
    m_addressType[AddressType::Email] = "email";
    m_addressType[AddressType::Facebook] = "facebook";
    m_addressType[AddressType::Instagram] = "instagram";
    m_addressType[AddressType::Phone] = "phone";
    m_addressType[AddressType::Twitter] = "twitter";
    m_addressType[AddressType::WhatsApp] = "whatsapp";
}


void AppLogFetcher::disableAnalytics() {
    m_analytics.setEnabled(false);
}


void AppLogFetcher::record(QString const& event, QString const& context)
{
    if (m_compressor) {
        m_analytics.record(event, context);
    }
}


void AppLogFetcher::securityCheck() {
    QtConcurrent::run(checkSigningHash);
}


void AppLogFetcher::onFinished()
{
    Report result = m_future.result();

    foreach (AppLaunch a, result.appLaunches)
    {
        record( QString("%1_%2_fully_visible").arg(a.appName).arg(a.launchType), QString::number(a.fullyVisible) );
        record( QString("%1_%2_process_created").arg(a.appName).arg(a.launchType), QString::number(a.processCreated) );
        record( QString("%1_%2_window_posted").arg(a.appName).arg(a.launchType), QString::number(a.windowPosted) );
    }

    foreach (QString const& removedApp, result.removedApps.keys() ) {
        record( "RemovedApp", QString("%1_%2").arg(removedApp).arg( result.removedApps.value(removedApp) ) );
    }

    bool silentFail = ( result.type == ReportType::BugReportAuto || result.type == ReportType::Periodic ) && ( !m_network.online() || result.data.isEmpty() );
    bool showFailure = result.type == ReportType::BugReportManual && ( !m_network.online() || result.data.isEmpty() );

    if (showFailure) {
        if ( !m_network.online() ) {
            emit submitted( tr("Report could not be submitted because you are offline! Please reconnect to the network and try again...") );
        } else {
            emit submitted( tr("Could not submit report! Please try again...") );
        }
    } else if (!silentFail) {
        QUrl url;
        url.setScheme("http");
        url.setUserName("user");
        url.setPassword("password");
        url.setHost("host");

        ReportType::Type t = (ReportType::Type)result.type;
        url.setPath( m_endpoint.value(t) );

        if (t == ReportType::BugReportManual) {
            url.addQueryItem("user_triggered", "1");
        }

        url.addQueryItem( "app", QCoreApplication::applicationName().toLower().remove( QRegExp("[^a-zA-Z\\d\\s:]") ).replace(" ", "_") );

        if ( !result.addresses.isEmpty() )
        {
            QVariantList allAddresses;
            QString buffer;

            foreach (Address const& a, result.addresses)
            {
                QString aType = m_addressType.value(a.type);

                if ( !aType.isEmpty() && !a.value.isEmpty() )
                {
                    QVariantMap qvm;
                    qvm["type"] = aType;
                    qvm["value"] = a.value.trimmed();
                    allAddresses << qvm;
                }
            }

            m_json.saveToBuffer(allAddresses, &buffer);
            result.params.insert("addresses", buffer);
        }

        QStringList keys = result.params.keys();
        foreach (QString const& key, keys) {
            url.addQueryItem( key, result.params.value(key) );
        }

        QVariantMap qvm;
        qvm[COOKIE_REPORT_TYPE] = t;

        if (t == ReportType::BugReportAuto || t == ReportType::Custom) {
            qvm[KEY_REPORT_NOTES] = result.params.value(KEY_REPORT_NOTES); // because we'll need to keep track of the notes to avoid duplicates
        }

        m_network.upload( url, QString("%1.zip").arg(result.id), result.data, qvm );
    } else {
        LOGGER("SilentFailure!");
    }

    if ( !m_queue.isEmpty() ) {
        submitReport( m_queue.dequeue() );
    }
}


void AppLogFetcher::onRequestComplete(QVariant const& c, QByteArray const& data, bool error)
{
    QVariantMap cookie = c.toMap();
    LOGGER(error << cookie << QString(data));

    if ( cookie.contains(COOKIE_REPORT_TYPE) ) {
        QVariantMap actualResult = m_json.loadFromBuffer(data).toMap();
        QString id = actualResult.value("report_id").toString();
        ReportType::Type t = (ReportType::Type)cookie.value(COOKIE_REPORT_TYPE).toInt();
        QString httpResult = actualResult.value("result").toString();
        bool success = !error && (httpResult == HTTP_RESPONSE_OK);
        QString message = !success ? tr("Could not submit logs! Please try again...") : tr("Logs have been submitted. Please provide the support team the following ID (copied to the clipboard): %1").arg(id);

        if (httpResult == "409 Conflict") {
            m_settings->setFlag(KEY_USER_ID); // reset it (in case it was deleted from the server for some odd reason), i would do an immediate FirstInstall report here but I fear that there might be a cycle that transpires!
            message += tr("\n\nYou may have to close the app and re-open it to correct the error.");
        }

        if (success)
        {
            if (t == ReportType::BugReportManual)
            {
                Clipboard clipboard;
                clipboard.clear();
                clipboard.insert("text/plain", Persistance::convertToUtf8(id) );

                emit submitted(message);
            } else if (t == ReportType::Periodic) {
                m_analytics.reset();
                m_settings->setFlag( KEY_ANALYTICS_COLLECTED, QDateTime::currentMSecsSinceEpoch() );

                QString version = actualResult.value("latest_version").toString();
                bool shouldSubmit = actualResult.value("submitBugReport").toInt() == 1;

                if ( !version.isEmpty() && QRegExp("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$").exactMatch(version) ) {
                    emit latestAppVersionFound(version);
                } else {
                    LOGGER("InvalidVersionFound" << version);
                }

                if (shouldSubmit)
                {
                    Report r(ReportType::BugReportAuto);
                    r.params.insert("notes", "CRA");
                    submitReport(r);
                }
            } else if (t == ReportType::AppVersionDiff) {
                //m_settings->setFlag( KEY_INSTALLED_APP_VERSION, QCoreApplication::instance()->applicationVersion() ); // already done in performCII()
            } else if (t == ReportType::OsVersionDiff) {
                m_settings->setFlag( KEY_LAST_OS_VERSION, bb::platform::PlatformInfo().osVersion() );
            } else if (t == ReportType::FirstInstall) {
                QString userId = actualResult.value("user_id").toString();
                m_settings->setFlag(KEY_USER_ID, userId);
                m_settings->setFlag(KEY_INSTALLED_APP_VERSION, QCoreApplication::instance()->applicationVersion() );
                m_settings->setFlag(KEY_LAST_OS_VERSION, bb::platform::PlatformInfo().osVersion() );
                int privilege = actualResult.value("privilege").toInt();

                if (privilege > 0) {
                    m_settings->setFlag(KEY_ADMIN_MODE, privilege);
                }
            } else if (t == ReportType::BugReportAuto) {
                QString notes = cookie.value(KEY_REPORT_NOTES).toString();
                notes = QString::number( qHash(notes) );
                m_settings->setFlag( DUPLICATE_KEY(notes), 1 );
            }

            emit syncComplete(t);
        } else {
            if (t == ReportType::BugReportManual) {
                emit submitted(message);
            } else if (t == ReportType::FirstInstall || t == ReportType::Periodic) {
                if (httpResult == "403 IllegalAccess") {
                    bb::Application::instance()->requestExit();
                }
            }

            LOGGER("NetworkErrorDuringSubmission" << actualResult);
        }
    }
}


int AppLogFetcher::performCII(int analyticDiffDays)
{
    int result = -1;
    qint64 launched = QDateTime::currentMSecsSinceEpoch();
    record( "AppLaunch", QString::number(launched) );

    if ( !m_settings->containsFlag(KEY_FIRST_INSTALL) ) {
        m_settings->setFlag( KEY_FIRST_INSTALL, launched );
    }

#if defined(QT_DEBUG)
    if ( m_settings->getFlag(KEY_INSTALLED_APP_VERSION).toString() != QCoreApplication::instance()->applicationVersion() )
    {
        m_settings->setFlag( KEY_INSTALLED_APP_VERSION, QCoreApplication::instance()->applicationVersion() );
        m_settings->setFlag(KEY_LAST_UPGRADE, launched);
        m_settings->setFlag(KEY_USER_ID, "78475");
    }

    return result;
#endif

    if ( !adminEnabled() && m_network.online() )
    {
        QString appVersion = QCoreApplication::instance()->applicationVersion();

        if ( !m_settings->containsFlag(KEY_USER_ID) )
        {
            m_settings->setFlag(KEY_LAST_UPGRADE, launched);
            m_settings->setFlag(KEY_INSTALLED_APP_VERSION, appVersion);
            result = doSubmit(ReportType::FirstInstall);
        } else {
            QString osVersion = bb::platform::PlatformInfo().osVersion();

            if ( m_settings->getFlag(KEY_LAST_OS_VERSION).toString() != osVersion ) {
                result = doSubmit(ReportType::OsVersionDiff);
            } else if ( upgradedApp() ) {
                m_settings->setFlag(KEY_LAST_UPGRADE, launched);
                m_settings->setFlag(DUPLICATE_GROUP);
                m_settings->setFlag(KEY_INSTALLED_APP_VERSION, appVersion);

                result = doSubmit(ReportType::AppVersionDiff);
            } else if ( m_settings->isUpdateNeeded(KEY_ANALYTICS_COLLECTED, analyticDiffDays) ) {
                m_analytics.commitStats();
                result = doSubmit(ReportType::Periodic);
            }
        }
    }

    return result;
}


int AppLogFetcher::doSubmit(ReportType::Type r)
{
    submitReport( Report(r) );
    return r;
}


bool AppLogFetcher::adminEnabled() const {
    return m_settings->getFlag(KEY_ADMIN_MODE) == 1;
}


bool AppLogFetcher::online() const {
    return m_network.online();
}


AppLogFetcher* AppLogFetcher::create(Persistance* settings, CompressFiles func, QObject* parent, bool dumpAll)
{
    if (!instance) {
        instance = new AppLogFetcher(settings, func, parent, dumpAll);
    }

    return instance;
}


AppLogFetcher* AppLogFetcher::getInstance() {
    return instance;
}


bool AppLogFetcher::deferredCheck(QString const& key, qint64 diff, bool versionBased)
{
    if (versionBased) {
        return m_settings->getFlag(key).toString() != QCoreApplication::applicationVersion() && m_settings->isUpdateNeeded(KEY_LAST_UPGRADE, diff) && online();
    } else {
        return !m_settings->containsFlag(key) && m_settings->isUpdateNeeded(KEY_FIRST_INSTALL, diff) && online();
    }
}


void AppLogFetcher::submitAttributes(QVariantMap const& attributes)
{
    Report r(ReportType::Attribute);

    foreach (QString const& key, attributes.keys()) {
        r.params.insert( key, attributes.value(key).toString() );
    }

    submitReport(r);
}


void AppLogFetcher::submitReport(QString const& name, QString const& email, QString const& notes, QStringList const& attachments)
{
    if ( !name.isEmpty() ) {
        m_settings->setFlag("cached_user_name", name);
    }

    if ( !email.isEmpty() ) {
        m_settings->setFlag("cached_user_email", email);
    }

    Report r(ReportType::BugReportManual);
    r.params.insert("sender_name", name);
    r.params.insert("sender_email", email);
    r.params.insert(KEY_REPORT_NOTES, notes);
    r.attachments.append(attachments);

    submitReport(r);
}


void AppLogFetcher::submitReport(Report r)
{
    return;
    if (m_compressor)
    {
        r.dumpAll = m_dumpAll;

        if ( !m_future.isRunning() ) {
            QFuture<Report> future = QtConcurrent::run(ReportGenerator::generate, m_compressor, r);
            m_future.setFuture(future);
        } else {
            m_queue << r;
        }
    }
}


void AppLogFetcher::onErrorMessage(const char* msg) {
    getInstance()->onError(msg);
}


void AppLogFetcher::onError(QString const& error)
{
    QString hashValue = QString::number( qHash(error) );

    if ( !m_settings->containsFlag( DUPLICATE_KEY(hashValue) ) )
    {
        Report r(ReportType::BugReportAuto);
        r.params.insert(KEY_REPORT_NOTES, error);
        submitReport(r);
    }
}


bool AppLogFetcher::upgradedApp() {
    return m_settings->getFlag(KEY_INSTALLED_APP_VERSION).toString() != QCoreApplication::instance()->applicationVersion();
}


AppLogFetcher::~AppLogFetcher()
{
}

} /* namespace canadainc */
