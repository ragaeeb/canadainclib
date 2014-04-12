#ifndef LOGMONITOR_H_
#define LOGMONITOR_H_

#include <QFileSystemWatcher>
#include <QSettings>

namespace canadainc {

class LogMonitor : public QObject
{
    Q_OBJECT

    QFileSystemWatcher m_settingsWatcher;
    QString m_logFile;
    QString m_key;

private slots:
    void settingChanged(QString const& file);

public:
    LogMonitor(QString const& key, QString const& logFile, QObject* parent=NULL);
    virtual ~LogMonitor();
};

} /* namespace canadainc */

#endif /* LOGMONITOR_H_ */
