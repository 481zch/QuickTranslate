#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

class NetWork : public QObject {
    Q_OBJECT
public:
    // 初始化
    explicit NetWork(QString appid, QString appkey, QObject *parent = nullptr);
    // 设置一些参数
    void setParameters(QString text, QString fromLanguage, QString toLanguage);
    // 文本翻译
    void translateText();
    // 预热
    void warmUp();
    void startWarmUp();

signals:
    // 传递翻译文本
    void translateFinish(QString result);

private slots:
    void onTranslationFinished();

private:
    QString text;
    QString fromLanguage;
    QString toLanguage;
    QThread workerThread;

    // 接入信息
    QString appid;
    QString appkey;
    QString urlStr = "https://api.fanyi.baidu.com/api/trans/vip/translate";

    // 网络请求接收
    QNetworkAccessManager manager;
    QNetworkReply *currentReply = nullptr;
};

#endif // NETWORK_H
