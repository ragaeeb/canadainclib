#ifndef LOGGER_H_
#define LOGGER_H_

#include <QString>
#include <QDebug>

#define LOGGER(a) qDebug() << "===" << __FILE__ << __LINE__ << __FUNCTION__ << a

extern void registerLogging(QString const& key);

#endif /* LOGGER_H_ */
