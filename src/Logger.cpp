#include "Logger.h"

#include <QDir>

namespace {

FILE* f = NULL;

void redirectedMessageOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type);

    if (f) {
        fprintf(f, "%s\n", msg);
    }
}

}

void registerLogging()
{
    f = fopen( QString( QDir::currentPath()+"/logs/ui.log").toUtf8().constData(), "w");
    qInstallMsgHandler(redirectedMessageOutput);
}


void deregisterLogging()
{
    if (f) {
        fclose(f);
        f = NULL;
    }
}
