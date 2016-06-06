#include "stdin.hpp"

#include <iostream>
#include <QTimer>
#include <QStringList>
#include <QCoreApplication>

class Stdin::Private
{
public:
  Private(int interval, Stdin *parent);
  ~Private();

  Stdin      *m_parent;
  QTimer     *m_timer;
  int         m_interval;
  QStringList m_buffer;
};

Stdin::Private::Private(int interval, Stdin *parent)
  : m_parent(parent)
  , m_timer(new QTimer(parent))
  , m_interval(interval)
{
}

Stdin::Private::~Private()
{
}

Stdin::Stdin(int interval, QObject *parent)
  : QObject(parent)
  , d(new Private(interval, this))
{
  connect(d->m_timer, SIGNAL(timeout()), this, SLOT(forwardOutput()));
}

Stdin::~Stdin()
{
  delete d;
}

void Stdin::fetch()
{
  readLine();
  d->m_timer->start();
}

void Stdin::readLine()
{
  for (std::string line; std::getline(std::cin, line);) {
    d->m_buffer << QString::fromUtf8(line.c_str());
  }
}

void Stdin::forwardOutput()
{
  readLine();
  if (!d->m_buffer.isEmpty()) {
    const QString &line = d->m_buffer.takeFirst();
    std::cout << line.toStdString() << std::endl;
  } else {
    if (std::cin.eof()) {
      QCoreApplication::exit();
    }
  }
  d->m_timer->start(d->m_interval);
}

#include "stdin.moc"
