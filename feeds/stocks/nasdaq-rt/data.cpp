#include "data.hpp"

#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QCoreApplication>
#include <QNetworkAccessManager>

#define USER_AGENT "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36"

class Data::Private {
public:
  Private(Data *parent);
  ~Private();

  QUrl            buildUrl();
  QNetworkRequest buildRequest();

public:
  QTimer                *m_timer;
  QString                m_symbol;
  int                    m_interval;
  QNetworkAccessManager *m_network;
};

Data::Private::Private(Data *parent)
  : m_timer(new QTimer(parent))
  , m_network(new QNetworkAccessManager(parent))
{
}

Data::Private::~Private() {
}

QUrl Data::Private::buildUrl() {
  return QUrl("http://www.nasdaq.com/symbol/" + m_symbol.toLower() + "/real-time");
}

QNetworkRequest Data::Private::buildRequest() {
  QNetworkRequest networkRequest(buildUrl());
  networkRequest.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
  networkRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
  return networkRequest;
}

Data::Data(const QString &symbol, int interval, QObject *parent)
  : QObject(parent)
  , d(new Private(this))
{
  d->m_symbol = symbol;
  d->m_interval = interval;
  connect(d->m_timer, SIGNAL(timeout()), this, SLOT(fetchData()));
  connect(d->m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(readResponse(QNetworkReply*)));
}

Data::~Data()
{
  delete d;
}

void Data::fetch()
{
  d->m_timer->start();
}

void Data::fetchData()
{
  d->m_network->get(d->buildRequest());
  d->m_timer->start(d->m_interval);
}

void Data::readResponse(QNetworkReply *reply)
{
  const QString &response = reply->readAll();
  QRegExp rx("<div id=\"qwidget_lastsale\" class=\"qwidget-dollar\">\\$([.\\d]+)</div>");
  if (rx.indexIn(response) != -1) {
    QVariantMap data;
    data["symbol"] = d->m_symbol;
    data["timestamp"] = QDateTime::currentDateTimeUtc().toTime_t();
    data["value"] = rx.cap(1);
    data["currency"] = "USD";
    QVariantMap root;
    root["type"] = "fact";
    root["data"] = data;
    QJsonDocument document(QJsonObject::fromVariantMap(root));
    std::cout << document.toJson(QJsonDocument::Compact).constData() << std::endl;
  }
  reply->deleteLater();
}

#include "data.moc"
