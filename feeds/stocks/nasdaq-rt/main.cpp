#include <QtCore>

#define APP_NAME    "sm-nasdaq-rt"
#define APP_VERSION "1.0"

#include "data.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(APP_NAME);
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption symbolsToBeTrackedOption(QStringList() << "s" << "symbols",
                                                "Symbols to be tracked",
                                                "symbols");
    parser.addOption(symbolsToBeTrackedOption);

    QCommandLineOption intervalsToTrackOption(QStringList() << "i" << "intervals",
                                              "Intervals to track selected symbols (milliseconds)",
                                              "intervals");
    parser.addOption(intervalsToTrackOption);

    parser.process(app);

    const QStringList &symbolsToBeTracked = parser.values(symbolsToBeTrackedOption);
    if (symbolsToBeTracked.isEmpty()) {
      std::cout << "No symbols were given" << std::endl;
      return 1;
    }

    const QStringList &intervalsToTrack = parser.values(intervalsToTrackOption);
    if (intervalsToTrack.count() != symbolsToBeTracked.count()) {
      std::cout << "The number of intervals need to match the number of symbols" << std::endl;
      return 1;
    }

    for (int i = 0; i < symbolsToBeTracked.count(); ++i) {
      Data *data = new Data(symbolsToBeTracked[i], intervalsToTrack[i].toInt(), &app);
      data->fetch();
    }

    return app.exec();
}
