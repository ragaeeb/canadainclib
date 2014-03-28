#ifndef LOGGER_H_
#define LOGGER_H_

#if defined(QT_NO_DEBUG)
#include <QDebug>

#define LOGGER(a) qDebug() << "==============" << __TIME__ << __FILE__ << __LINE__ << __FUNCTION__ << a
#else
#define LOGGER(a)
#endif

extern void registerLogging();
extern void deregisterLogging();

#endif /* LOGGER_H_ */
