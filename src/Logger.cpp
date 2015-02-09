#include "Logger.h"

#include <QDir>

#include <slog2.h>

namespace {

FILE* f = NULL;
const char* cached_file_name = NULL;
bool frozen = false;
slog2_buffer_set_config_t   buffer_config;
slog2_buffer_t              buffer_handle[2];

void redirectedMessageOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type);
    fprintf(f ? f : stdout, "%s\n", msg);
    slog2c( buffer_handle[0], 0, SLOG2_DEBUG1, msg );

#if !defined(QT_NO_DEBUG)
    fflush(f ? f : stdout);
#endif
}

}

void registerLogging(QString const& fileName, bool melt)
{
    if ( f || (frozen && !melt) ) {
        return;
    }

    if ( !fileName.isNull() ) {
        cached_file_name = fileName.toUtf8().constData();
        f = fopen(cached_file_name, "w");

        buffer_config.num_buffers = 1;
        buffer_config.verbosity_level = SLOG2_DEBUG1;
        buffer_config.buffer_set_name = "xyz";
        buffer_config.buffer_config[0].buffer_name = "xyz_fast";
        buffer_config.buffer_config[0].num_pages = 7;

        slog2_register( &buffer_config, buffer_handle, 0 );
    } else if (cached_file_name) {
        f = fopen(cached_file_name, "w");
    }

    qInstallMsgHandler(redirectedMessageOutput);
}


const char* logFilePath() {
    return cached_file_name;
}


void deregisterLogging(bool freeze)
{
    frozen = freeze;

    if (f) {
        fclose(f);
        f = NULL;
        qInstallMsgHandler(0);
    }
}
