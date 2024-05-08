#include "network.h"
#include <QDebug>
#include <QCryptographicHash>

NetWork::NetWork(QString appid, QString appkey, QObject *parent)
    : QObject(parent), appid(appid), appkey(appkey), manager(this) {
//    warmUp();
}

void NetWork::setParameters(QString text, QString fromLanguage, QString toLanguage) {
    this->text = text;
    this->fromLanguage = fromLanguage;
    this->toLanguage = toLanguage;
}

void NetWork::translateText() {
    qDebug() << "appid:" << appid << "appkey:" << appkey << "text:" << text << "from:" << fromLanguage << "to:" << toLanguage;

    // 避免两个访问冲突
    if (currentReply) {
        currentReply->abort();
    }

    int salt = rand() % 10000 + 1;
    QString sign = QString(QCryptographicHash::hash((appid + text + QString::number(salt) + appkey).toUtf8(), QCryptographicHash::Md5).toHex());

    QUrl url(urlStr);
    QUrlQuery query;
    query.addQueryItem("q", text);
    query.addQueryItem("from", fromLanguage);
    query.addQueryItem("to", toLanguage);
    query.addQueryItem("appid", appid);
    query.addQueryItem("salt", QString::number(salt));
    query.addQueryItem("sign", sign);
    url.setQuery(query);

    QNetworkRequest request(url);
    currentReply = manager.get(request);
    connect(currentReply, &QNetworkReply::finished, this, &NetWork::onTranslationFinished);
}

void NetWork::onTranslationFinished() {
    if (currentReply->error() == QNetworkReply::NoError) {
        QByteArray responseBytes = currentReply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseBytes);
        QJsonObject jsonObject = jsonResponse.object();
        QString resultText;
        QJsonValue translations = jsonObject["trans_result"];
        if (!translations.isNull()) {
            QJsonArray translationArray = translations.toArray();
            for (const QJsonValue &value : translationArray) {
                QJsonObject obj = value.toObject();
                resultText += obj["dst"].toString();
            }
        }
        qDebug() << "完成了翻译，翻译内容为：" << resultText;
        emit translateFinish(resultText);
    } else {
        qDebug() << "Error during translation:" << currentReply->errorString();
        emit translateFinish("");
    }
    currentReply->deleteLater();
    currentReply = nullptr;
}

void NetWork::warmUp() {
    QUrl url("https://api.fanyi.baidu.com/api/trans/vip/translate");
    QUrlQuery query;
    query.addQueryItem("q", "test");
    query.addQueryItem("from", "en");
    query.addQueryItem("to", "zh");
    query.addQueryItem("appid", appid);
    url.setQuery(query);

    QNetworkRequest request(url);
    // 这里只是发送请求，不处理回复
    manager.get(request);
    qDebug() << "warmUp() executed in thread:" << QThread::currentThreadId();
}

void NetWork::startWarmUp()
{
    setParent(nullptr);
    moveToThread(&workerThread);
    connect(&workerThread, &QThread::started, this, &NetWork::warmUp);
    workerThread.start();
}
