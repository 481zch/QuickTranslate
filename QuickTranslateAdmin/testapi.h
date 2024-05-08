#ifndef TESTAPI_H
#define TESTAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class TestApi : public QObject
{
    Q_OBJECT
public:
    explicit TestApi(QString appid, QString appkey, QObject *parent = nullptr);
    ~TestApi();
public slots:
    void testNet();

signals:
    void testSuccess(bool isValid);

private:
    QString urlStr = "https://fanyi-api.baidu.com/api/trans/vip/language";
    QString testText = "hello";
    QNetworkAccessManager manager;
    QString appid;
    QString appkey;
};

#endif // TESTAPI_H
