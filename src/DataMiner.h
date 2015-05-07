#ifndef DATAMINER_H_
#define DATAMINER_H_

#include "IOUtils.h"

#include <bb/Application>
#include <bb/MemoryInfo>
#include <bb/PackageInfo>

#include <bb/device/BatteryInfo>
#include <bb/device/HardwareInfo>
#include <bb/device/SimCardInfo>

#include <bb/platform/PlatformInfo>

#include <bb/system/InvokeManager>

#include <sys/utsname.h>

#define CAMERA_PPS_PATH "/pps/system/camera/status"
#define LINE_SEPARATOR "\n"

namespace canadainc {

using namespace bb::device;
using namespace bb::system;

QString extractAfterFirstLine(QString const& input)
{
    int index = input.indexOf(LINE_SEPARATOR);

    if (index >= 0) {
        return input.mid(index+1).trimmed();
    }

    return QString();
}

QString extractSpacedOutWithHeader(QString const& file)
{
    QString result = extractAfterFirstLine( IOUtils::readTextFile(file).trimmed() );

    return !result.isNull() ? file+NEW_LINE+result+NEW_LINE : result;
}

QString getLastCapturedScreenshot(bool preview)
{
    QString result;
    /*
    QString lastCapturedText = "LastCapturedFile::";
    QStringList data = canadainc::IOUtils::readTextFile(CAMERA_PPS_PATH).split(LINE_SEPARATOR);

    foreach (QString current, data)
    {
        if ( current.startsWith(lastCapturedText) )
        {
            result = current.mid( lastCapturedText.length() );
            break;
        }
    } */

    QDir selectedDir("/accounts/1000/shared/camera");

    if ( selectedDir.exists() )
    {
        QStringList all = selectedDir.entryList( QStringList() << "IMG_*.png", QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot, QDir::Time | QDir::Name );

        if ( !all.isEmpty() ) {
            result = QString("%1/%2").arg( selectedDir.path() ).arg( all.first() );
        }
    }

    if ( preview && !result.isEmpty() )
    {
        bb::system::InvokeRequest request;
        request.setAction("bb.action.VIEW");
        request.setTarget("sys.pictures.card.previewer");
        request.setUri( QUrl::fromLocalFile(result) );

        bb::system::InvokeManager().invoke(request);
    }

    return result;
}

void fetchInfoForInterface(QString const& name, QString& ipv6, QStringList& result)
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
            result << QString("%1=%2").arg(name).arg(ipv4);
        }
    }
}

QStringList captureUserInfo()
{
    QStringList userInfo;

    QString ipv6;
    fetchInfoForInterface("bcm0", ipv6, userInfo);
    fetchInfoForInterface("msm0", ipv6, userInfo);
    fetchInfoForInterface("bptp0", ipv6, userInfo);

    SimCardInfo sci;

    if ( !sci.serialNumber().isEmpty() ) {
        userInfo << QString("SimSerial: %1").arg( sci.serialNumber() );
    }

    return userInfo;
}

QStringList captureDeviceInfo(utsname const& udata)
{
    QStringList deviceInfo;
    deviceInfo << QString("applicationName: %1").arg( QCoreApplication::applicationName() );
    deviceInfo << QString("applicationVersion: %1").arg( QCoreApplication::applicationVersion() );

    deviceInfo << QString("NodeName: %1").arg(udata.nodename);

    bb::MemoryInfo m;
    deviceInfo << QString("availableDeviceMemory: %1").arg( m.availableDeviceMemory() );
    deviceInfo << QString("memoryUsedByCurrentProcess: %1").arg( m.memoryUsedByCurrentProcess() );
    deviceInfo << QString("totalDeviceMemory: %1").arg( m.totalDeviceMemory() );

    bb::platform::PlatformInfo p;
    deviceInfo << QString("os: %1").arg( p.osVersion() );

    HardwareInfo hw;
    deviceInfo << QString("DeviceName: %1").arg( hw.deviceName() );
    deviceInfo << QString("InternalDevice: %1").arg( QFile::exists("/pps/system/quip_public/status") );
    deviceInfo << QString("Machine: %1").arg(udata.machine);
    deviceInfo << QString("ModelName: %1").arg( hw.modelName() );
    deviceInfo << QString("ModelNumber: %1").arg( hw.modelNumber() );
    deviceInfo << QString("OSCreation: %1").arg(udata.version);
    deviceInfo << QString("PhysicalKeyboard: %1").arg( hw.isPhysicalKeyboardDevice() );

    if ( !hw.pin().isEmpty() )
    {
        SimCardInfo sci;
        deviceInfo << QString("SimCountryCode: %1").arg( sci.mobileCountryCode() );
        deviceInfo << QString("SimNetworkCode: %1").arg( sci.mobileNetworkCode() );
    }

    BatteryInfo b;
    deviceInfo << QString("BatteryChargingState: %1").arg( b.chargingState() );
    deviceInfo << QString("BatteryCycleCount: %1").arg( b.cycleCount() );
    deviceInfo << QString("BatteryTemperature: %1").arg( b.temperature() );

    deviceInfo << extractSpacedOutWithHeader("/base/board.tdf");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/BattMgr/status");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/bfx/status");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/bluetooth/settings");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/cellular/radioctrl/status_public");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/cellular/sms/status");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/cellular/uicc/card0/status_public");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/clock/status");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/font/settings");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/ims/status_public");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/input/options");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/locale/settings");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/networking/all/status_public");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/paymentsystem/status_public");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/radioctrl/modem0/status_public");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/rum/csm/status_public");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/unifiedradiomgr/status");
    deviceInfo << extractSpacedOutWithHeader("/pps/services/wifi/status_public");
    deviceInfo << extractSpacedOutWithHeader("/pps/system/gfx/display0/settings");
    deviceInfo << extractSpacedOutWithHeader("/pps/system/nvram/deviceinfo");
    deviceInfo << extractSpacedOutWithHeader("/pps/system/settings");
    deviceInfo << extractSpacedOutWithHeader(CAMERA_PPS_PATH);

    return deviceInfo;
}

void checkSigningHash()
{
    QString signingHash = bb::PackageInfo().authorSigningHash();

    if ( /*!signingHash.startsWith("test") && */signingHash != "gYAAgL4hrHsGI4tljUxgYhylQfs" ) {
        bb::Application::instance()->requestExit();
    }
}

} /* namespace canadainc */

#endif /* DATAMINER_H_ */
