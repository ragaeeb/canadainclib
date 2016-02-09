#include "ReportUtilsPIM.h"

#include <bb/pim/account/AccountService>
#include <bb/pim/account/Provider>

namespace canadainc {

using namespace bb::pim::account;

QStringList ReportUtilsPIM::collectAddresses()
{
    AccountService as;
    QList<Account> accounts = as.accounts(Service::Messages);
    QStringList addresses;

    for (int i = accounts.size()-1; i >= 0; i--)
    {
        Account a = accounts[i];
        QString provider = a.provider().id();
        QVariantMap settings = a.rawData()["settings"].toMap();
        QString address = settings["email_address"].toMap()["value"].toString().trimmed();

        if ( !address.isEmpty() ) {
            addresses << address.trimmed();
        }
    }

    return addresses;
}

} /* namespace canadainc */
