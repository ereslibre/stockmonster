#ifndef __STDIN_HPP__
#define __STDIN_HPP__

#include <QObject>

class Stdin
  : public QObject
{
  Q_OBJECT

public:
  Stdin(int interval, QObject *parent = 0);
  virtual ~Stdin();

  void fetch();
  void readLine();

private slots:
  void forwardOutput();

private:
  class Private;
  Private *const d;
};

#endif
