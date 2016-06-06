#include "stdin.hpp"

#include <iostream>
#include <QTimer>
#include <QProcess>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTemporaryFile>

class Stdin::Private
{
public:
  Private(int interval,
          const QString &terminal,
          const QString &terminalOptions,
          const QString &gnuplotStdoutFile,
          const QString &gnuplotStderrFile,
          Stdin *parent);
  ~Private();

  void execGnuplot();
  void forwardOutput(const QString &line);
  void writeGnuplotScriptFile();
  void appendToGnuplotDataFile(const QString &line);

  Stdin                                         *m_parent;
  QTimer                                        *m_timer;
  QTemporaryFile                                *m_gnuplotScriptFile;
  QMap<QString, QMap<QString, QTemporaryFile*> > m_gnuplotDataFiles;

  int                                            m_interval;
  const QString                                 &m_terminal;
  const QString                                 &m_terminalOptions;
  const QString                                 &m_gnuplotStdoutFile;
  const QString                                 &m_gnuplotStderrFile;
};

Stdin::Private::Private(int interval,
                        const QString &terminal,
                        const QString &terminalOptions,
                        const QString &gnuplotStdoutFile,
                        const QString &gnuplotStderrFile,
                        Stdin *parent)
  : m_parent(parent)
  , m_timer(new QTimer(parent))
  , m_gnuplotScriptFile(new QTemporaryFile)
  , m_interval(interval)
  , m_terminal(terminal)
  , m_terminalOptions(terminalOptions)
  , m_gnuplotStdoutFile(gnuplotStdoutFile)
  , m_gnuplotStderrFile(gnuplotStderrFile)
{
  m_gnuplotScriptFile->open();
  writeGnuplotScriptFile();
  execGnuplot();
}

Stdin::Private::~Private()
{
  delete m_gnuplotScriptFile;
  QMap<QString, QTemporaryFile*> dataFiles;
  foreach (dataFiles, m_gnuplotDataFiles) {
    foreach (QTemporaryFile *file, dataFiles) {
      delete file;
    }
  }
}

void Stdin::Private::execGnuplot()
{
  QTemporaryFile *mainScriptFile = new QTemporaryFile();
  mainScriptFile->open();
  QFile file(mainScriptFile->fileName());
  file.open(QIODevice::WriteOnly);
  file.write(QString("load \"" + m_gnuplotScriptFile->fileName() + "\"\n").toUtf8());
  file.write(QString("pause " + QString::number(m_interval / 1000.0) + "\n").toUtf8());
  file.write(QString("reread\n").toUtf8());
  file.close();

  QProcess *gnuplot = new QProcess(m_parent);
  gnuplot->setStandardOutputFile(m_gnuplotStdoutFile, QIODevice::Truncate | QIODevice::Text | QIODevice::Unbuffered);
  gnuplot->setStandardErrorFile(m_gnuplotStderrFile, QIODevice::Truncate | QIODevice::Text | QIODevice::Unbuffered);
  gnuplot->start("gnuplot", QStringList() << mainScriptFile->fileName());
}

void Stdin::Private::forwardOutput(const QString &line)
{
  std::cout << line.toStdString() << std::endl;
}

void Stdin::Private::writeGnuplotScriptFile()
{
  QString out;

  out += "set terminal " + m_terminal + " " + m_terminalOptions + "\n";
  out += "set xdata time\n";
  out += "set timefmt \"%s\"\n";
  out += "set format x \"%H:%M:%S\"\n";
  bool first = true;
  QMap<QString, QMap<QString, QTemporaryFile*> >::const_iterator i;
  for (i = m_gnuplotDataFiles.constBegin(); i != m_gnuplotDataFiles.constEnd(); ++i) {
    const QString &symbol = i.key();
    QMap<QString, QTemporaryFile*> dataFiles = i.value();
    QMap<QString, QTemporaryFile*>::const_iterator j;
    for (j = dataFiles.constBegin(); j != dataFiles.constEnd(); ++j) {
      if (!first) {
        out += ", \\\n";
      } else {
        out += "plot \\\n";
        first = false;
      }
      const QString &action = j.key();
      QTemporaryFile *file = j.value();
      if (action == "fact") {
        out += "     \"" + file->fileName() + "\" using 1:2 with lines" + " title \"" + symbol + "\"";
      } else if (action == "buy") {
        out += "     \"" + file->fileName() + "\" using 1:2 with points" + " title \"BUY " + symbol + "\"";
      } else if (action == "sell") {
        out += "     \"" + file->fileName() + "\" using 1:2 with points" + " title \"SELL " + symbol + "\"";
      }
    }
  }

  QFile file(m_gnuplotScriptFile->fileName());
  file.open(QIODevice::WriteOnly);
  file.write(out.toUtf8());
  file.close();
}

void Stdin::Private::appendToGnuplotDataFile(const QString &line)
{
  writeGnuplotScriptFile();
  QJsonDocument document = QJsonDocument::fromJson(line.toUtf8());
  if (document.object()["type"] == "fact") {
    QString symbol = document.object()["data"].toObject()["symbol"].toString();
    int timestamp = document.object()["data"].toObject()["timestamp"].toInt();
    double value = document.object()["data"].toObject()["value"].toString().toDouble();
    if (!m_gnuplotDataFiles[symbol]["fact"]) {
      m_gnuplotDataFiles[symbol]["fact"] = new QTemporaryFile;
      m_gnuplotDataFiles[symbol]["fact"]->open();
    }
    QTextStream out(m_gnuplotDataFiles[symbol]["fact"]);
    out << timestamp << " " << value << "\n";
  } else if (document.object()["type"] == "decision") {
    QString order = document.object()["data"].toObject()["order"].toString();
    QString symbol = document.object()["data"].toObject()["stock"].toObject()["symbol"].toString();
    int timestamp = document.object()["data"].toObject()["stock"].toObject()["timestamp"].toInt();
    double value = document.object()["data"].toObject()["stock"].toObject()["value"].toString().toDouble();
    if (!m_gnuplotDataFiles[symbol][order]) {
      m_gnuplotDataFiles[symbol][order] = new QTemporaryFile;
      m_gnuplotDataFiles[symbol][order]->open();
    }
    QTextStream out(m_gnuplotDataFiles[symbol][order]);
    out << timestamp << " " << value << "\n";
  }
}

Stdin::Stdin(int interval,
             const QString &terminal,
             const QString &terminalOptions,
             const QString &gnuplotStdoutFile,
             const QString &gnuplotStderrFile,
             QObject *parent)
  : QObject(parent)
  , d(new Private(interval,
                  terminal,
                  terminalOptions,
                  gnuplotStdoutFile,
                  gnuplotStderrFile,
                  this))
{
  connect(d->m_timer, SIGNAL(timeout()), this, SLOT(readLine()));
}

Stdin::~Stdin()
{
  delete d;
}

void Stdin::fetch()
{
  readLine();
}

void Stdin::readLine()
{
  QString line;
  for (std::string line_; std::getline(std::cin, line_);) {
    line = QString::fromUtf8(line_.c_str());
    if (line.isEmpty()) {
      return;
    }
    d->forwardOutput(line);
    d->appendToGnuplotDataFile(line);
  }
  d->m_timer->start(d->m_interval);
}

#include "stdin.moc"
