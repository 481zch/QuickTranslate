#ifndef GETLANGUAGE_H
#define GETLANGUAGE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

class GetLanguage : public QObject
{
    Q_OBJECT
public:
    GetLanguage(QString appkey, QString appid, QObject *parent = nullptr);
    void achieveLanguage(QString s);
    void warmUp();
    void startWarmUp();
    ~GetLanguage();
signals:
    void returnLanguage(QString res);

private:
    QString urlStr = "https://fanyi-api.baidu.com/api/trans/vip/language";
    QNetworkAccessManager manager;
    QString appid;
    QString appkey;
    QThread workerThread;
};

#endif // GETLANGUAGE_H
