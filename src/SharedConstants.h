#ifndef SHAREDCONSTANTS_H_
#define SHAREDCONSTANTS_H_

#define NO_CALENDAR_ACCESS !bb::pim::calendar::CalendarService().settings().isValid()
#define NO_CONTACTS_ACCESS bb::pim::contacts::ContactService().count( bb::pim::contacts::ContactListFilters() ) == 0
#define NAME_FIELD(var,field) QString("replace( replace( replace( replace( coalesce(%1.displayName, TRIM((coalesce(%1.prefix,'') || ' ' || coalesce(%1.kunya,'') || ' ' || %1.name))),\"'\",''), '%2', ''), '%3', ''), '  ', ' ' ) AS %4").arg(var).arg( QChar(8217) ).arg( QChar(8216) ).arg(field)

#endif /* SHAREDCONSTANTS_H_ */
