#include "getlanguage.h"
#include <QDebug>

GetLanguage::GetLanguage(QString appid, QString appkey, QObject *parent): QObject(parent)
{
    this->appid = appid;
    this->appkey = appkey;
}

void GetLanguage::achieveLanguage(QString s)
{
    int salt = rand() % 10000 + 1;
    QString sign = QString(QCryptographicHash::hash((this->appid + s + QString::number(salt) + this->appkey).toUtf8(), QCryptographicHash::Md5).toHex());

    QUrl url(this->urlStr);
    QUrlQuery query;
    query.addQueryItem("q", s);
    query.addQueryItem("appid", this->appid);
    query.addQueryItem("salt", QString::number(salt));
    query.addQueryItem("sign", sign);
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseBytes = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseBytes);
            QJsonObject jsonObject = jsonResponse.object();
            QString src;

            if(jsonObject["error_msg"].toString() == "success") {
                // 从JSON响应中提取src值
                QJsonObject dataObject = jsonObject["data"].toObject();
                src = dataObject["src"].toString();
                // 发出带有src值的信号
                emit returnLanguage(src);
            }
        }
        reply->deleteLater();
    });
}


void GetLanguage::warmUp() {
    QUrl url(urlStr);
    QUrlQuery query;
    query.addQueryItem("q", "test");
    query.addQueryItem("from", "en");
    query.addQueryItem("to", "zh");
    query.addQueryItem("appid", appid);
    url.setQuery(query);

    QNetworkRequest request(url);
    manager.get(request);
}

void GetLanguage::startWarmUp()
{
    setParent(nullptr);
    moveToThread(&workerThread);
    connect(&workerThread, &QThread::started, this, &GetLanguage::warmUp);
    workerThread.start();
}

GetLanguage::~GetLanguage() {
    workerThread.quit();
    workerThread.wait();
}

