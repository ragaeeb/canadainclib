#include "AccountImporter.h"
#include "Logger.h"

#include "bbndk.h"

#include <bb/pim/account/AccountService>
#include <bb/pim/account/Provider>

#include <bb/system/phone/Phone>

#if BBNDK_VERSION_AT_LEAST(10,3,0)
#include <bb/pim/phone/CallHistoryService>
#endif

namespace canadainc {

using namespace bb::pim::account;
using namespace bb::system::phone;

AccountImporter::AccountImporter(Service::Type type, bool loadPhone) :
        m_type(type), m_loadPhone(loadPhone)
{
}


void AccountImporter::run()
{
    AccountService as;
    QList<Account> accounts = as.accounts(m_type);
    QVariantList result;

    for (int i = accounts.size()-1; i >= 0; i--)
    {
    	QVariantMap settings = accounts[i].rawData()["settings"].toMap();
    	QString address = settings["email_address"].toMap()["value"].toString();
    	QString name = settings["email_display_name"].toMap()["value"].toString();

    	if ( name.isEmpty() ) {
    		name = settings["display_name"].toMap()["value"].toString();
    	}

    	if ( name.isEmpty() && accounts[i].provider().id() == "sms-mms" ) {
    		name = tr("SMS");
    	}

    	if ( name.isEmpty() && accounts[i].id() == 1 ) {
    	    name = tr("Local");
    	}

    	QVariantMap current;
    	current["address"] = address;
    	current["name"] = name;
    	current["accountId"] = accounts[i].id();

    	LOGGER( "[account]" << accounts[i].id() << accounts[i].provider().id() << name );

    	result << current;
    }

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    if (m_loadPhone)
    {
        QMap<QString, Line> lines = Phone().lines();

        QVariantMap current;
        current["address"] = lines["cellular"].address();
        current["name"] = tr("Cellular");
        current["accountId"] = bb::pim::phone::CallHistoryService::defaultAccount().id();
        current["isCellular"] = true;

        LOGGER(current);

        result << current;
    }
#endif

    emit importCompleted(result);
}


AccountImporter::~AccountImporter()
{
}

} /* namespace canadainc */
