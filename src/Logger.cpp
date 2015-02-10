#include "Logger.h"

#include <QCoreApplication>
#include <QDir>

#include <slog2.h>

namespace {

FILE* f = NULL;
slog2_buffer_set_config_t buffer_config;
slog2_buffer_t buffer_handle[1];

void redirectedMessageOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type);

#if !defined(QT_NO_DEBUG)
    fprintf(f ? f : stdout, "%s\n", msg);
    fflush(f ? f : stdout);
#endif
    _Uint8t severity = SLOG2_DEBUG1;

    switch ( type ) {
    case QtWarningMsg:
        severity = SLOG2_ERROR;
        break;
    case QtCriticalMsg:
        severity = SLOG2_CRITICAL;
        break;
    case QtFatalMsg:
        severity = SLOG2_SHUTDOWN;
        break;
    default:
        break;
    }

    slog2c( buffer_handle[0], 0, severity, msg );
}

}

void registerLogging(QString const& key)
{
#if !defined(QT_NO_DEBUG)
    const char* cached_file_name = QString("%1/logs/%2.log").arg( QDir::currentPath() ).arg(key).toUtf8().constData();
    f = fopen(cached_file_name, "w");
#endif
    buffer_config.num_buffers = 1;
    buffer_config.verbosity_level = SLOG2_DEBUG1;
    buffer_config.buffer_set_name = QCoreApplication::applicationName().toUtf8().constData();
    buffer_config.buffer_config[0].buffer_name = key.toUtf8().constData();
    buffer_config.buffer_config[0].num_pages = 7;
    slog2_register( &buffer_config, buffer_handle, 0 );

    qInstallMsgHandler(redirectedMessageOutput);
}
