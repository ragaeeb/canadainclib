#include "Logger.h"
#include "PimContactPickerSheet.h"

#include <bb/pim/contacts/ContactService>

#include <bb/cascades/pickers/ContactPicker>

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

}

namespace canadainc {

using namespace bb::cascades::pickers;

PimContactPickerSheet::PimContactPickerSheet(QObject* parent) : QObject(parent), m_picker(NULL), m_filterMobile(true)
{
}


PimContactPickerSheet::~PimContactPickerSheet()
{
}


void PimContactPickerSheet::open()
{
	if (m_picker == NULL)
	{
		m_picker = new ContactPicker(this);
		connect( m_picker, SIGNAL( contactSelected(int) ), this, SLOT( onContactSelected(int) ) );
		connect( m_picker, SIGNAL( canceled() ), this, SIGNAL( canceled() ) );

		if (m_filterMobile)
		{
			QSet<AttributeKind::Type> filters;
			filters << AttributeKind::Phone;
			KindSubKindSpecifier mobileFilter(AttributeKind::Phone, AttributeSubKind::PhoneMobile);
			QSet<KindSubKindSpecifier> subkindFilters;
			subkindFilters << mobileFilter;
			m_picker->setSubKindFilters(subkindFilters);
			m_picker->setKindFilters(filters);
		}
	}

	m_picker->open();
}


bool PimContactPickerSheet::filterMobile() const {
	return m_filterMobile;
}


void PimContactPickerSheet::setFilterMobile(bool filter) {
	m_filterMobile = filter;
}

void PimContactPickerSheet::onContactSelected(int id)
{
	LOGGER("onContactSelected()" << id);

	Contact contact = ContactService().contactDetails(id);

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

	emit contactSelected(result);
}

}
