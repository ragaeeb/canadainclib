#include "ReportUtilsPhone.h"

#include <bb/system/phone/Phone>

namespace canadainc {

using namespace bb::system::phone;

QStringList ReportUtilsPhone::collectNumbers()
{
    QStringList addresses;
    QMap<QString, Line> lines = Phone().lines();

    QString address = lines["cellular"].address().trimmed();

    if ( !address.isEmpty() ) {
        addresses << address;
    }

    return addresses;
}

} /* namespace canadainc */
