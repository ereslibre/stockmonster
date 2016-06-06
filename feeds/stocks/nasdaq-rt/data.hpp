#ifndef __DATA_HPP__
#define __DATA_HPP__

#include <iostream>
#include <QObject>

class QNetworkReply;

class Data
  : public QObject
{
  Q_OBJECT

public:
  Data(const QString &symbol, int interval, QObject *parent = 0);
  virtual ~Data();

  void fetch();

private slots:
  void fetchData();
  void readResponse(QNetworkReply *reply);

private:
  class Private;
  Private *const d;
};

#endif
