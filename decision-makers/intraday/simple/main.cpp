#include <QtCore>

#define APP_NAME    "sm-intraday-simple"
#define APP_VERSION "1.0"

#include "stdin.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(APP_NAME);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    Stdin *stdin = new Stdin(&app);
    stdin->fetch();

    return 0;
}
