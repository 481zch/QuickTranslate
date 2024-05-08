#include "testapi.h"

TestApi::TestApi(QString appid, QString appkey, QObject *parent) : QObject(parent), appid(appid), appkey(appkey) {}

TestApi::~TestApi(){}

void TestApi::testNet() {
    int salt = rand() % 10000 + 1;
    QString sign = QString(QCryptographicHash::hash((this->appid + this->testText + QString::number(salt) + this->appkey).toUtf8(), QCryptographicHash::Md5).toHex());

    QUrl url(this->urlStr);
    QUrlQuery query;
    query.addQueryItem("q", this->testText);
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

            if(jsonObject["error_code"].toInt() == 0 && jsonObject["error_msg"].toString() == "success") {
                // 如果error_code为0，并且error_msg为success，则发射验证成功的信号
                emit testSuccess(true);
            } else {
                // 否则发射错误信号
                emit testSuccess(false);
            }
        } else {
            emit testSuccess(false);
        }
        reply->deleteLater();
    });
}
