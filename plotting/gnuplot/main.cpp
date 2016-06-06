#include <QtCore>

#define APP_NAME    "sm-gnuplot"
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
                                      "Interval to plot",
                                      "interval",
                                      "1000");
    parser.addOption(intervalOption);

    QCommandLineOption terminalOption(QStringList() << "t" << "terminal",
                                      "gnuplot terminal to be used",
                                      "terminal",
                                      "qt");
    parser.addOption(terminalOption);

    QCommandLineOption terminalOptionsOption(QStringList() << "o" << "terminal-options",
                                             "gnuplot terminal options to be used",
                                             "terminal options");
    parser.addOption(terminalOptionsOption);

    QCommandLineOption gnuplotStdoutFileOption("gnuplot-stdout-file",
                                               "gnuplot stdout file",
                                               "file",
                                               "/dev/null");
    parser.addOption(gnuplotStdoutFileOption);

    QCommandLineOption gnuplotStderrFileOption("gnuplot-stderr-file",
                                               "gnuplot stderr file",
                                               "file",
                                               "/dev/null");
    parser.addOption(gnuplotStderrFileOption);

    parser.process(app);

    int interval = parser.value(intervalOption).toInt();
    const QString &terminal = parser.value(terminalOption);
    const QString &terminalOptions = parser.value(terminalOptionsOption);
    const QString &gnuplotStdoutFile = parser.value(gnuplotStdoutFileOption);
    const QString &gnuplotStderrFile = parser.value(gnuplotStderrFileOption);

    Stdin *stdin = new Stdin(interval,
                             terminal,
                             terminalOptions,
                             gnuplotStdoutFile,
                             gnuplotStderrFile,
                             &app);
    stdin->fetch();

    return app.exec();
}
