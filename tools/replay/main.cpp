#include <QtCore>

#define APP_NAME    "sm-replay"
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

    QCommandLineOption intervalOption(QStringList() << "i" << "interval",
                                      "Minimum interval to print data (milliseconds)",
                                      "interval",
                                      "1000");
    parser.addOption(intervalOption);

    parser.process(app);

    Stdin *stdin = new Stdin(parser.value(intervalOption).toInt(), &app);
    stdin->fetch();

    return app.exec();
}
