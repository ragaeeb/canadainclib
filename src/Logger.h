#ifndef LOGGER_H_
#define LOGGER_H_

#include <QString>
#include <QDebug>

#define LOGGER(a) qDebug() << "===" << __FILE__ << __LINE__ << __FUNCTION__ << a
typedef void (*ErrorMessageHandler)(const char* errorMessage);

extern void registerLogging(QString const& key, ErrorMessageHandler errorHandler=NULL);

#endif /* LOGGER_H_ */
