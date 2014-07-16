#include "LogMonitor.h"
#include "AppLogFetcher.h"
#include "Logger.h"

#include <QDateTime>
#include <QFile>

#define ONE_SECOND 1000
#define ONE_MINUTE ONE_SECOND*60
#define INTERVAL 30*ONE_MINUTE
#define KB 1024
#define MB 1024*KB
#define MAX_LOG_SIZE 10*MB

namespace canadainc {

void LogMonitor::create(QString const& key, QString const& logFile, QObject* parent, bool force) {
    new LogMonitor(key, logFile, parent, force);
}

LogMonitor::LogMonitor(QString const& key, QString const& logFile, QObject* parent, bool force) :
        QObject(parent), m_logFile(logFile), m_key(key), m_forced(force)
{
    QSettings settings;

    m_settingsWatcher.addPath( settings.fileName() );
    connect( &m_settingsWatcher, SIGNAL( fileChanged(QString const&) ), this, SLOT( settingChanged(QString const&) ) );

    m_timer.setSingleShot(false);
    m_timer.setInterval(INTERVAL);
    connect( &m_timer, SIGNAL( timeout() ), this, SLOT( timeout() ) );

    if ( settings.value(key).toBool() || force ) {
        registerLogging(logFile);
        m_timer.start();
    }
}


void LogMonitor::settingChanged(QString const& file)
{
    Q_UNUSED(file);

    QSettings settings;
    settings.sync();

    if ( settings.contains(STOP_LOGGING_KEY) ) {
        deregisterLogging(true);
        m_timer.stop();
    } else if ( settings.contains(START_LOGGING_KEY) ) {

        if ( settings.value(m_key).toBool() )
        {
            registerLogging(m_logFile, true);

            if ( !m_timer.isActive() ) {
                m_timer.start();
            }
        }
    } else {
        bool allowLogging = settings.value(m_key).toBool();

        if ( settings.contains(m_key) )
        {
            if (allowLogging || m_forced)
            {
                registerLogging(m_logFile);

                if ( !m_timer.isActive() ) {
                    m_timer.start();
                }
            } else {
                LOGGER("deregister" << allowLogging << settings.contains(m_key) << m_key);
                deregisterLogging();
                m_timer.stop();
            }
        }
    }
}


void LogMonitor::timeout()
{
    if ( QFile(m_logFile).size() > MAX_LOG_SIZE )
    {
        deregisterLogging();
        registerLogging(m_logFile);
        LOGGER("Log truncated at " << QDateTime::currentDateTime() );
    }
}


LogMonitor::~LogMonitor()
{
}

} /* namespace canadainc */
