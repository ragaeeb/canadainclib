#ifndef SHAREDCONSTANTS_H_
#define SHAREDCONSTANTS_H_

#define NAME_FIELD(var) QString("replace( replace( replace( replace( coalesce(%1.displayName, TRIM((coalesce(%1.prefix,'') || ' ' || coalesce(%1.kunya,'') || ' ' || %1.name))),\"'\",''), '%2', ''), '%3', ''), '  ', ' ' )").arg(var).arg( QChar(8217) ).arg( QChar(8216) )

#endif /* SHAREDCONSTANTS_H_ */
