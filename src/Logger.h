#ifndef LOGGER_H_
#define LOGGER_H_

#include <QDebug>

#define CARD_LOG "app_card"
//#define DEBUG_RELEASE 1
#define LOGGER(a) qDebug() << "===" << __FILE__ << __LINE__ << __FUNCTION__ << a
#define QRC true
#define SERVICE_LOG "app_service"
#define UI_LOG "app_ui"
typedef void (*ErrorMessageHandler)(const char* errorMessage);

extern void registerLogging(const char* key, ErrorMessageHandler errorHandler=NULL);
extern void setErrorHandler(ErrorMessageHandler errorHandler=NULL);

#endif /* LOGGER_H_ */
