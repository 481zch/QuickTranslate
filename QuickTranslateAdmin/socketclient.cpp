#include "socketclient.h"

SocketClient::SocketClient(QObject *parent)
    : QObject{parent}, socket(new QTcpSocket(this))
{
    successed = false;
    connect(socket, &QTcpSocket::connected, this, &SocketClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &SocketClient::onReadyRead);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &SocketClient::onError);
}

SocketClient::~SocketClient()
{
    socket->disconnectFromHost();
}

void SocketClient::connectToServer(const QString &host, quint16 port)
{
    socket->connectToHost(host, port);
}

void SocketClient::sendMessage(const QByteArray&message)
{
    // 会发送三种类型的信息，为json格式，已经封装完成
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->write(message);
    }
}

void SocketClient::onConnected()
{
    qDebug() << "Connected to the server.";
    successed = true;
    emit connectSucess();
}

void SocketClient::onReadyRead()
{
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    emit receivedMessage(message);
}

void SocketClient::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << socketError << socket->errorString();
}
