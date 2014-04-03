#include "Logger.h"

#include <QDir>

namespace {

FILE* f = NULL;
const char* cached_file_name = NULL;

void redirectedMessageOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type);
    fprintf(f ? f : stderr, "%s\n", msg);
}

}

void registerLogging(QString const& fileName)
{
    if (f) {
        deregisterLogging();
    }

    if ( !fileName.isNull() ) {
        cached_file_name = QString( QString("%1/logs/%2").arg( QDir::currentPath() ).arg(fileName) ).toUtf8().constData();
        f = fopen(cached_file_name, "w");
    } else if (cached_file_name) {
        f = fopen(cached_file_name, "w");
    }

    qInstallMsgHandler(redirectedMessageOutput);
}


void deregisterLogging()
{
    if (f) {
        fclose(f);
        f = NULL;
    }
}
