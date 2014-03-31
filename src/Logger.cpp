#include "Logger.h"

#include <QDir>

namespace {

FILE* f = NULL;

void redirectedMessageOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type);
    fprintf(f ? f : stderr, "%s\n", msg);
}

}

void registerLogging(QString const& fileName)
{
    if ( !fileName.isNull() ) {
        f = fopen( QString( QString("%1/logs/%2").arg( QDir::currentPath() ).arg(fileName) ).toUtf8().constData(), "w");
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
