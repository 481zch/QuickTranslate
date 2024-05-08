#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(QObject *parent = nullptr);
    ~SocketClient();

    // 连接服务器
    void connectToServer(const QString &host, quint16 port);
    void sendMessage(const QByteArray &message);
    bool connectState() { return successed;}

signals:
    void receivedMessage(const QString &message);
    void connectSucess();   // 成功建立连接

private slots:
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError);

private:
    QTcpSocket *socket;
    bool successed;
};

#endif // SOCKETCLIENT_H
