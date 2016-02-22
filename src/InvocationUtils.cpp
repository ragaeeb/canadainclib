#include "InvocationUtils.h"
#include "Logger.h"
#include "SharedConstants.h"

#include <bb/PpsObject>

#include <sys/utsname.h>

namespace canadainc {

InvocationUtils::InvocationUtils(QObject* parent) : QObject(parent)
{
}


InvocationUtils::~InvocationUtils()
{
}

void InvocationUtils::launchSettingsApp(QString const& key, QVariantMap const& metadata, InvokeManager* invokeManager)
{
    InvokeRequest request;
    request.setTarget("sys.settings.target");
    request.setAction("bb.action.OPEN");
    request.setMimeType("settings/view");
    request.setUri( QUrl("settings://"+key) );

    if ( !metadata.isEmpty() ) {
        request.setMetadata(metadata);
    }

    invokeManager->invoke(request);
}


bool InvocationUtils::hasLocationAccess()
{
    QFile target("/pps/services/geolocation/status");

    if ( !target.open(QIODevice::ReadOnly) ) {
        LOGGER("NoLocationAccess");
        return false;
    }

    target.close();

    return true;
}


bool InvocationUtils::hasSharedFolderAccess()
{
    QString sdDirectory = QString("/accounts/1000/shared/downloads");

    if ( !QDir(sdDirectory).exists() )
    {
        LOGGER(sdDirectory << "DidNotExist!");
        return false;
    }

    return true;
}


bool InvocationUtils::hasEmailSmsAccess()
{
    if ( QFile(SMS_DB_PATH_LEGACY).exists() || QFile(SMS_DB_PATH).exists() ) {
        return true;
    } else {
        struct utsname udata;
        uname(&udata);

        if ( QString(udata.machine).startsWith("x86") ) { // simulator build doesn't have it
            return true;
        }
    }

    LOGGER("NoEmailSmsAccess");
    return false;
}


bool InvocationUtils::hasPhoneControlAccess()
{
    if ( QFile("/pps/services/phone/protected/status").exists() ) {
        return true;
    }

    LOGGER("NoPhoneControlAccess");
    return false;
}


InvokeTargetReply* InvocationUtils::invoke(QString const& targetId, QString const& action, QString const& mime, QString const& uri, QString const& data, InvokeManager* invokeManager)
{
    InvokeRequest request;
    request.setTarget(targetId);
    request.setAction(action);
    request.setUri(uri);
    request.setMimeType(mime);
    request.setData( data.toUtf8() );

    return invokeManager->invoke(request);
}


void InvocationUtils::call(QString const& number, InvokeManager* invokeManager)
{
    LOGGER(number);
    QVariantMap map;
    map.insert("number", number);
    QByteArray requestData = bb::PpsObject::encode(map, NULL);

    bb::system::InvokeRequest request;
    request.setAction("bb.action.DIAL");
    request.setMimeType("application/vnd.blackberry.phone.startcall");
    request.setData(requestData);

    invokeManager->invoke(request);
}


} /* namespace canadainc */
