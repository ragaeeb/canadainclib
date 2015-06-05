#include "Logger.h"

#include <QCoreApplication>
#include <QDir>

#include <slog2.h>
#include <cstring>

#define DEBUG_RELEASE 1

namespace {

#if defined(QT_DEBUG) || DEBUG_RELEASE
ErrorMessageHandler errorHandler = NULL;
FILE* f = NULL;
#endif

slog2_buffer_set_config_t buffer_config;
slog2_buffer_t buffer_handle[1];

void redirectedMessageOutput(QtMsgType type, const char* msg)
{
#if defined(QT_DEBUG) || DEBUG_RELEASE
    fprintf(f ? f : stdout, "%s\n", msg);
    fflush(f ? f : stdout);
#endif
    _Uint8t severity = SLOG2_DEBUG1;

    switch (type)
    {
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

    slog2c(buffer_handle[0], 0, severity, msg);

#if defined(QT_DEBUG) || DEBUG_RELEASE
    if ( (type == QtWarningMsg || type == QtCriticalMsg) && errorHandler && strstr(msg, "ReferenceError") ) {
        errorHandler(msg);
    }
#endif
}

}

void registerLogging(QString const& key, ErrorMessageHandler handler)
{
#if defined(QT_DEBUG) || DEBUG_RELEASE
    const char* cached_file_name = QString("%1/logs/%2.log").arg( QDir::currentPath() ).arg(key).toUtf8().constData();
    f = fopen(cached_file_name, "w");
    errorHandler = handler;
#else
    Q_UNUSED(handler);
#endif

    buffer_config.num_buffers = 1;
    buffer_config.verbosity_level = SLOG2_DEBUG1;
    buffer_config.buffer_set_name = QCoreApplication::applicationName().toUtf8().constData();
    //buffer_config.buffer_config[0].buffer_name = key.toUtf8().constData();
    buffer_config.buffer_config[0].num_pages = 7;
    slog2_register(&buffer_config, buffer_handle, 0);

    qInstallMsgHandler(redirectedMessageOutput);
}
