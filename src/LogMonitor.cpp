#include "LogMonitor.h"
#include "AppLogFetcher.h"
#include "Logger.h"

#include <QDateTime>

namespace canadainc {

LogMonitor::LogMonitor(QString const& key, QString const& logFile, QObject* parent) :
        QObject(parent), m_logFile(logFile), m_key(key)
{
    QSettings m_settings;

    m_settingsWatcher.addPath( m_settings.fileName() );

    connect( &m_settingsWatcher, SIGNAL( fileChanged(QString const&) ), this, SLOT( settingChanged(QString const&) ) );

    if ( m_settings.value(key).toBool() ) {
        registerLogging(logFile);
    }
}


void LogMonitor::settingChanged(QString const& file)
{
    Q_UNUSED(file);

    LOGGER(QDateTime::currentDateTime() << file);

    QSettings m_settings;
    m_settings.sync();

    if ( m_settings.contains(STOP_LOGGING_KEY) ) {
        LOGGER("stop logging key");
        deregisterLogging(true);
    } else if ( m_settings.contains(START_LOGGING_KEY) ) {
        LOGGER("start logging key");
        if ( m_settings.value(m_key).toBool() ) {
            LOGGER("register logging");
            registerLogging(m_logFile, true);
        }
    } else {
        bool allowLogging = m_settings.value(m_key).toBool();

        LOGGER("allowLogging" << allowLogging << m_settings.contains(m_key) << m_key);

        if ( m_settings.contains(m_key) )
        {
            if (allowLogging) {
                LOGGER("registerLogging" << allowLogging << m_settings.contains(m_key) << m_key);
                registerLogging(m_logFile);
            } else {
                LOGGER("deregister" << allowLogging << m_settings.contains(m_key) << m_key);
                deregisterLogging();
            }
        }
    }
}


LogMonitor::~LogMonitor()
{
}

} /* namespace canadainc */
