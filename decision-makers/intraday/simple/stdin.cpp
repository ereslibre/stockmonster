#include "stdin.hpp"

#include <iostream>
#include <QList>
#include <QDateTime>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonDocument>

class Stdin::Private
{
public:
  Private(Stdin *parent);
  ~Private();

  void forwardOutput(const QString &line);
  void processData(const QString &line);
  void emitOrder(const QString &order, const QString &symbol, double price, const QString &currency, int timestamp);

  Stdin *m_parent;

  double m_lastCheck1;
  double m_lastCheck2;

  double m_priceBought;
};

Stdin::Private::Private(Stdin *parent)
  : m_parent(parent)
  , m_lastCheck1(0)
  , m_lastCheck2(0)
  , m_priceBought(0)
{
}

Stdin::Private::~Private()
{
}

void Stdin::Private::forwardOutput(const QString &line)
{
  std::cout << line.toStdString() << std::endl;
}

void Stdin::Private::processData(const QString &line)
{
  QJsonDocument document = QJsonDocument::fromJson(line.toUtf8());
  const QString &symbol = document.object()["data"].toObject()["symbol"].toString();
  const QString &currency = document.object()["data"].toObject()["currency"].toString();
  int timestamp = document.object()["data"].toObject()["timestamp"].toInt();
  double currentPrice = document.object()["data"].toObject()["value"].toString().toDouble();

  if (m_lastCheck1 == 0) {
    m_lastCheck1 = currentPrice;
    return;
  }

  if (m_lastCheck2 == 0) {
    m_lastCheck2 = m_lastCheck1;
    return;
  }

  if (m_priceBought) {
    if (currentPrice > m_lastCheck1 && currentPrice > m_lastCheck2 && m_lastCheck1 > m_lastCheck2 && currentPrice > m_priceBought) {
      emitOrder("sell", symbol, currentPrice, currency, timestamp);
      m_priceBought = 0;
    }
  } else {
    if (currentPrice < m_lastCheck1 && currentPrice < m_lastCheck2 && m_lastCheck1 < m_lastCheck2) {
      emitOrder("buy", symbol, currentPrice, currency, timestamp);
      m_priceBought = currentPrice;
    }
  }

  m_lastCheck2 = m_lastCheck1;
  m_lastCheck1 = currentPrice;
}

void Stdin::Private::emitOrder(const QString &order, const QString &symbol, double price, const QString &currency, int timestamp)
{
  QVariantMap data;
  data["order"] = order;
  QVariantMap stock;
  stock["symbol"] = symbol;
  stock["timestamp"] = timestamp;
  stock["value"] = QString::number(price);
  stock["currency"] = currency;
  if (m_priceBought) {
    stock["diff"] = QString::number(price - m_priceBought);
  }
  data["stock"] = stock;
  QVariantMap root;
  root["type"] = "decision";
  root["data"] = data;
  QJsonDocument document(QJsonObject::fromVariantMap(root));
  std::cout << document.toJson(QJsonDocument::Compact).constData() << std::endl;
}

Stdin::Stdin(QObject *parent)
  : QObject(parent)
  , d(new Private(this))
{
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
  for (std::string line_; std::getline(std::cin, line_);) {
    const QString &line = QString::fromUtf8(line_.c_str());
    d->forwardOutput(line);
    d->processData(line);
  }
}

#include "stdin.moc"
