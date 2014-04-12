#include "Logger.h"

#include <QDir>

namespace {

FILE* f = NULL;
const char* cached_file_name = NULL;
bool frozen = false;

void redirectedMessageOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type);
    fprintf(f ? f : stderr, "%s\n", msg);
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
