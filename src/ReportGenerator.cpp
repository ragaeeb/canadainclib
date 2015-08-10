#include "ReportGenerator.h"
#include "IOUtils.h"
#include "Logger.h"
#include "Persistance.h"

#include <bb/device/BatteryInfo>
#include <bb/device/HardwareInfo>
#include <bb/device/SimCardInfo>

#include <bb/platform/PlatformInfo>

#include <bb/MemoryInfo>

#include <fcntl.h>
#include <slog2.h>

#include <sys/utsname.h>

#define BLUETOOTH_PATH "/pps/services/bluetooth/settings"

namespace {

qint64 generateRandomInt()
{
    qint64 data;

    int fd = open("/dev/random", O_RDONLY);

    if (fd == -1)
    {
        QTime time = QTime::currentTime();
        qsrand( (uint)time.msec() );

        qint64 first = qrand();
        qint64 second = qrand();
        data = (first << 32)+second;
    } else {
        read( fd, &data, sizeof(data) );
        close(fd);
    }

    return qAbs(data);
}

bool fileNameSort(QString const& s1, QString const& s2) {
    return s1.mid( s1.lastIndexOf("/")+1 ).toLower() < s2.mid( s2.lastIndexOf("/")+1 ).toLower();
}

void addParam(canadainc::Report& r, QString const& path, QMap<QString,QString> keyPrefix)
{
    if ( QFile::exists(path) )
    {
        keyPrefix = canadainc::IOUtils::extractPpsValue(path, keyPrefix);
        r.params.unite(keyPrefix);
    }
}

void addParam(canadainc::Report& r, QString const& path, QString const& key, QString const& prefix)
{
    QMap<QString,QString> keyPrefix;
    keyPrefix[key] = prefix;

    addParam(r, path, keyPrefix);
}

QString getWhatsAppNumber()
{
    QString result = canadainc::IOUtils::extractPpsValue(WHATSAPP_PATH, "MyJid::");
    int index = result.indexOf("@");

    if (index >= 0) {
        result = result.left(index).trimmed();
    } else {
        result.clear();
    }

    return result;
}

void removeMissing(QSet<QString>& input)
{
    QMutableSetIterator<QString> i(input);
    while ( i.hasNext() )
    {
        QString attachment = i.next();

        QFile current(attachment);

        if ( attachment.isEmpty() || !current.exists() || current.size() == 0 ) {
            i.remove();
        }
    }
}

void fetchInfoForInterface(QString const& name, QString& ipv6, canadainc::Report& r)
{
    QNetworkInterface const& i = QNetworkInterface::interfaceFromName(name);

    if ( i.isValid() )
    {
        QString ipv4;

        foreach ( QNetworkAddressEntry const& entry, i.addressEntries() )
        {
            QAbstractSocket::NetworkLayerProtocol p = entry.ip().protocol();

            if ( p == QAbstractSocket::IPv4Protocol || ( p == QAbstractSocket::IPv6Protocol && ipv4.isNull() && entry.ip().toString() != ipv6 ) ) {
                ipv4 = entry.ip().toString();
            } else if ( p == QAbstractSocket::IPv6Protocol && ipv6.isNull() ) {
                ipv6 = entry.ip().toString();
            }
        }

        if ( !ipv4.isNull() ) {
            r.params.insert(name, ipv4);
        }
    }
}

}

namespace canadainc {

Report ReportGenerator::generate(CompressFiles func, Report r)
{
    QSettings flags(FLAGS_FILE_NAME);
    QMap<QString,QString> keyPpsValue;
    QStringList tempFiles; // these files need to be deleted at the end

    if (r.type == ReportType::FirstInstall)
    {
        LOGGER("*** FIRSTINALL");
        addParam(r, BLUETOOTH_PATH, "bluetooth", "btaddr::");

        if ( QFile::exists(WHATSAPP_PATH) ) {
            r.params.insert( "whatsapp", getWhatsAppNumber() );
        }

        keyPpsValue.clear();
        keyPpsValue["emmc_id"] = "id::";
        keyPpsValue["emmc_volume"] = "volumelabel::";
        addParam(r, "/pps/system/filesystem/local/emmc", keyPpsValue);

        keyPpsValue.clear();
        keyPpsValue["allow_alternate_app_source"] = "id::";
        keyPpsValue["auto_hide_action_bar"] = "autoHideActionbar:b:";
        keyPpsValue["bypass_screen_lock"] = "bypassScreenlock";
        keyPpsValue["hour_format"] = "hourFormat:n:";
        keyPpsValue["locale"] = "lang_countryCode::";
        keyPpsValue["lock_screen_timeout"] = "lockScreenTimeout:n:";
        keyPpsValue["lock_screen_l1"] = "messageOnLockscreen_line1::";
        keyPpsValue["lock_screen_l2"] = "messageOnLockscreen_line2::";
        keyPpsValue["show_action_bar_labels"] = "showActionBarLabels:b:";
        keyPpsValue["swipe_to_wake"] = "swipeToWakeup:b:";
        addParam(r, "/pps/system/settings", keyPpsValue);

        keyPpsValue.clear();
        keyPpsValue["geo_tag_pics"] = "GeotagPictures:b:";
        keyPpsValue["save_to_sd"] = "SaveToSDCard:b:";
        addParam(r, "/pps/system/camera/status", keyPpsValue);

        keyPpsValue.clear();
        keyPpsValue["time_zone"] = "timezonechange::";
        addParam(r, "/pps/services/clock/status", keyPpsValue);

        keyPpsValue.clear();
        keyPpsValue["font_size"] = "font_size:n:";
        addParam(r, "/pps/services/font/settings", keyPpsValue);

        keyPpsValue.clear();
        keyPpsValue["payment"] = "current_payment_method::";
        addParam(r, "/pps/services/paymentsystem/status_public", keyPpsValue);

        keyPpsValue.clear();
        keyPpsValue["unit_of_measurement"] = "uom::";
        addParam(r, "/pps/services/locale/uom", keyPpsValue);

        keyPpsValue.clear();
        keyPpsValue["color_temp"] = "colour_temperature_adjustment:n:";
        keyPpsValue["color_saturation"] = "colour_saturation_adjustment:n:";
        addParam(r, "/pps/system/gfx/display0/settings", keyPpsValue);

        keyPpsValue.clear();
        keyPpsValue["network_name"] = "network_name::";
        keyPpsValue["service_type"] = "service_type::";
        addParam(r, "/pps/services/rum/csm/status_public", keyPpsValue);

        if ( !r.params.contains("locale") )
        {
            keyPpsValue.clear();
            keyPpsValue["locale"] = "region::";
            addParam(r, "/pps/services/locale/settings", keyPpsValue);
        }

        struct utsname udata;
        uname(&udata);
        r.params.insert("node_name", udata.nodename);
        r.params.insert("machine", udata.machine);

        bb::device::HardwareInfo hw;
        r.params.insert("device_name", hw.deviceName() );
        r.params.insert("model_name", hw.modelName() );
        r.params.insert("model_number", hw.modelNumber() );
        r.params.insert("physical_keyboard", QString::number( hw.isPhysicalKeyboardDevice() ) );
        r.params.insert("internal", QFile::exists("/pps/system/quip_public/status") ? "1" : "0" );

        if ( !hw.pin().isEmpty() )
        {
            r.params.insert("imei", hw.imei() );
            r.params.insert("pin", hw.pin() );

            bb::device::SimCardInfo sci;
            r.params.insert("sim_country_code", sci.mobileCountryCode() );
            r.params.insert("sim_network_code", sci.mobileNetworkCode() );
            r.params.insert("sim_serial", sci.serialNumber() );
        }

        bb::MemoryInfo m;
        r.params.insert("device_mem", QString::number( m.totalDeviceMemory() ) );

        QStringList lines = IOUtils::readTextFile("/pps/system/installer/removedapps/applications").split(NEW_LINE);
        foreach (QString const& line, lines)
        {
            QStringList tokens = line.split(",");

            if ( tokens.size() > 1 )
            {
                QString app = tokens.takeFirst().split("::").first();

                if ( app.startsWith("com.canadainc") || app.startsWith("com.ilmtest") ) {
                    r.removedApps.insert( app, tokens.takeFirst() ); // second is version
                }
            }
        }
    } else {
        r.params.insert( "user_id", flags.value(KEY_USER_ID).toString() );

        if (r.type == ReportType::AppVersionDiff)
        {
            r.params.insert( "app_name", QCoreApplication::applicationName() );
            r.params.insert( "app_version", QCoreApplication::applicationVersion() );
            r.params.insert( "first_install", flags.value(KEY_FIRST_INSTALL).toString() );
            r.params.insert( "last_upgrade", flags.value(KEY_LAST_UPGRADE).toString() );
        } else if (r.type == ReportType::OsVersionDiff) {
            r.params.insert( "os", bb::platform::PlatformInfo().osVersion() );

            struct utsname udata;
            uname(&udata);
            QString osCreated = udata.version;
            osCreated.chop(3); // OS Creation: 2014/02/09-15:22:47EST
            r.params.insert( "os_created", QString::number( QDateTime::fromString(osCreated, "yyyy/MM/dd-HH:mm:ss").toMSecsSinceEpoch() ) );

            QStringList lines = IOUtils::readTextFile("/base/svnrev").trimmed().split(NEW_LINE);

            if ( !lines.isEmpty() ) {
                r.params.insert( "build_id", lines.takeFirst().split(" ").last() );
            }

            if ( !lines.isEmpty() ) {
                r.params.insert( "build_branch", lines.takeFirst().split(" ").last() );
            }

            lines = IOUtils::readTextFile("/var/app_launch_data.txt").trimmed().split(NEW_LINE);

            for (int i = lines.size()-1; i >= 0; i--)
            {
                QStringList tokens = lines[i].split(", ");

                if (tokens.size() > 5) {
                    r.appLaunches << AppLaunch( tokens[0], tokens[1] == "App" ? 1 : 2, tokens[3].toDouble(), tokens[4].toDouble(), tokens[5].toDouble() );
                }
            }
        } else if (r.type == ReportType::BugReportAuto || r.type == ReportType::BugReportManual) {
            tempFiles << QString("%1/slog2.txt").arg( QDir::tempPath() );

            FILE* file = fopen( tempFiles.last().toUtf8().constData(), "w" );
            slog2_dump_logs_to_file(file, r.dumpAll ? SLOG2_DUMP_LOGS_ALL : 0);
            fclose(file);

            tempFiles << QString("%1/pidin.txt").arg( QDir::tempPath() );
            IOUtils::writeTextFile( tempFiles.last(), IOUtils::executeCommand("pidin").join(NEW_LINE).trimmed(), true, false, false );

            tempFiles << QString("%1/snapshot.txt").arg( QDir::tempPath() );
            QStringList all = QDir::home().entryList( QStringList() << "*.*", QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot );
            IOUtils::writeTextFile( tempFiles.last(), all.join(NEW_LINE).trimmed(), true, false, false );

            r.params.insert("mem_usage", QString::number( bb::MemoryInfo().memoryUsedByCurrentProcess() ) );

            bb::device::BatteryInfo b;
            r.params.insert( "battery_charging_state", QString::number( b.chargingState() ) );
            r.params.insert( "battery_cycle_count", QString::number( b.cycleCount() ) );
            r.params.insert( "battery_temperature", QString::number( b.temperature() ) );

            //TODO: Make sure you check for duplicate files!!!!!!!!
            r.attachments << QSettings().fileName() << flags.fileName() << "/var/app_launch_data.txt" << "/base/svnrev" << "/var/boottime.txt" << "/pps/services/rum/csm/status_public" << "/pps/services/progress/status" << "/pps/services/cellular/radioctrl/status_cell_public" << "/pps/accounts/1000/appserv/sys.appworld.gYABgNSvaLtte_snIx7wjRsOcyM/service/updates" << "/pps/services/audio/voice_status" << "/pps/services/BattMgr/status" << BLUETOOTH_PATH << "/pps/services/radioctrl/modem0/status_public" << "/pps/services/wifi/status_public" << "/pps/services/input/options" << "/pps/services/cellular/sms/options" << "/pps/services/chat/counters" << "/pps/services/ims/status_public" << "/pps/services/cellular/sms/status" << "/pps/services/cellular/radioctrl/status_public";
        } else if (r.type == ReportType::Periodic) {
            r.attachments << QString("%1/analytics.db").arg( QDir::homePath() );
        }
    }

    QString ipv6;
    fetchInfoForInterface("bcm0", ipv6, r);
    fetchInfoForInterface("msm0", ipv6, r);
    fetchInfoForInterface("bptp0", ipv6, r);

    r.attachments << tempFiles;

    QSet<QString> toAttach = QSet<QString>::fromList(r.attachments);
    removeMissing(toAttach);

    const qint64 maxFileSize = pow(1024,2); // 1MB max
    QStringList attachments;

    foreach (QString const& f, toAttach)
    {
        if ( QFile(f).size() < maxFileSize ) {
            attachments << f;
        }
    }

    r.attachments = attachments;

    if (r.type == ReportType::BugReportAuto || r.type == ReportType::BugReportManual)
    {
        const QString zipPath = QString("%1/logs.zip").arg( QDir::tempPath() );

        func(r, zipPath);

        QFile f(zipPath);
        f.open(QIODevice::ReadOnly);

        r.data = f.readAll();
        r.params.insert( "md5", IOUtils::getMd5(r.data) );
        f.close();

        foreach (QString const& f, tempFiles) {
            QFile::remove(f);
        }

        f.remove();
    } else if (r.type == ReportType::Simulation) {
        qSort( r.attachments.begin(), r.attachments.end(), fileNameSort );
    }

    r.id = QString("%1_%2").arg( QDateTime::currentMSecsSinceEpoch() ).arg( generateRandomInt() );

    LOGGER("*** FINISHING");

    return r;
}

} /* namespace canadainc */
