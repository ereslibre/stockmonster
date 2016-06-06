#ifndef __STDIN_HPP__
#define __STDIN_HPP__

#include <QObject>

class Stdin
  : public QObject
{
  Q_OBJECT

public:
  Stdin(int interval,
        const QString &terminal,
        const QString &terminalOptions,
        const QString &gnuplotStdoutFile,
        const QString &gnuplotStderrFile,
        QObject *parent = 0);
  virtual ~Stdin();

  void fetch();

private slots:
  void readLine();

private:
  class Private;
  Private *const d;
};

#endif
