#include "AccountImporter.h"
#include "Logger.h"

#include <bb/pim/account/AccountService>
#include <bb/pim/account/Provider>

namespace canadainc {

using namespace bb::pim::account;

AccountImporter::AccountImporter(Service::Type type) : m_type(type)
{
}


void AccountImporter::run()
{
	LOGGER("AccountImporter::run()" << m_type);

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

    	LOGGER("CURRENT ACCOUNT ID! >>>" << accounts[i].id() << accounts[i].provider().id() << "name" << name);

    	result.append(current);
    }

    emit importCompleted(result);
}


AccountImporter::~AccountImporter()
{
}

} /* namespace canadainc */
