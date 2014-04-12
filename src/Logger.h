#ifndef LOGGER_H_
#define LOGGER_H_

#include <QString>

//#define BETA 1

#if !defined(QT_NO_DEBUG)
#include <QDebug>

#define LOGGER(a) qDebug() << "===" << __TIME__ << __FILE__ << __LINE__ << __FUNCTION__ << a
#else
#define LOGGER(a)
#endif

extern void registerLogging(QString const& fileName=QString(), bool melt=false);
extern void deregisterLogging(bool freeze=false);
extern const char* logFilePath();

#endif /* LOGGER_H_ */
