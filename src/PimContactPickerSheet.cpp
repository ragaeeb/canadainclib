#include "PimContactPickerSheet.h"
#include "Logger.h"
#include "Persistance.h"
#include "PimUtil.h"

#include <bb/pim/contacts/ContactService>

#include <bb/cascades/pickers/ContactPicker>

#include <QtConcurrentRun>

using namespace bb::pim::contacts;

namespace {

QVariantList populateAttributes(QList<ContactAttribute> const& source, QString const& type)
{
	QVariantList numbersResult;

	for (int i = source.size()-1; i >= 0; i--)
	{
		QVariantMap phoneMap;
		phoneMap["name"] = source[i].attributeDisplayLabel();
		phoneMap["value"] = source[i].value();
		phoneMap["type"] = type;

		numbersResult << phoneMap;
	}

	return numbersResult;
}

QVariantList renderContacts(QList<int> const& ids)
{
    LOGGER("Rendering" << ids);
    ContactService cs;
    QVariantList all;

    for (int i = ids.size()-1; i >= 0; i--)
    {
        Contact contact = cs.contactDetails(ids[i]);

        QVariantMap result;
        result["displayName"] = contact.displayName();

        if ( !contact.smallPhotoFilepath().isEmpty() ) {
            result["smallPhotoPath"] = contact.smallPhotoFilepath();
        }

        QVariantList mediums;
        mediums.append( populateAttributes( contact.phoneNumbers(), "phone" ) );
        mediums.append( populateAttributes( contact.phoneNumbers(), "sms" ) );
        mediums.append( populateAttributes( contact.emails(), "email" ) );

        result["mediums"] = mediums;

        all << result;
    }

    LOGGER("Rendered" << all);

    return all;
}

}

namespace canadainc {

using namespace bb::cascades::pickers;

PimContactPickerSheet::PimContactPickerSheet(QObject* parent) :
        QObject(parent), m_mode(ContactSelectionMode::Single)
{
    connect( &m_future, SIGNAL( finished() ), this, SLOT( onRenderComplete() ) );
}


PimContactPickerSheet::~PimContactPickerSheet()
{
}


void PimContactPickerSheet::open()
{
    if ( !PimUtil::hasContactsAccess() ) {
        Persistance::showBlockingToast("Warning: It seems like the app does not have access to your contacts. If you leave this permission off, some features may not work properly. Select OK to launch the Application Permissions screen where you can turn these settings on.");
    }

    ContactPicker* picker = new ContactPicker(this);
    picker->setMode(m_mode);
    connect( picker, SIGNAL( contactSelected(int) ), this, SLOT( contactSelected(int) ) );
    connect( picker, SIGNAL( contactsSelected(const QList<int> &) ), this, SLOT( contactsSelected(QList<int> const&) ) );
    connect( picker, SIGNAL( canceled() ), this, SLOT( canceled() ) );

	picker->open();
}


void PimContactPickerSheet::contactsSelected(QList<int> const& contactIds)
{
    LOGGER(contactIds);
    QFuture<QVariantList> future = QtConcurrent::run(renderContacts, contactIds);
    m_future.setFuture(future);

    sender()->deleteLater();
}


void PimContactPickerSheet::contactSelected(int id) {
	contactsSelected(QList<int>() << id);
}


void PimContactPickerSheet::canceled()
{
    sender()->deleteLater();
    emit finished( QVariantList() );
}


void PimContactPickerSheet::onRenderComplete()
{
    QVariantList result = m_future.result();
    LOGGER("Render completed" << result);

    emit finished(result);
}


ContactSelectionMode::Type PimContactPickerSheet::mode() const {
    return m_mode;
}


void PimContactPickerSheet::setMode(ContactSelectionMode::Type mode) {
    m_mode = mode;
}

}
