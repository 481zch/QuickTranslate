#include "socketserver.h"

SocketServer::SocketServer(QObject *parent): QObject{parent}, tcpServer(nullptr)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &SocketServer::onNewConnection);
    startServer(12345);
}

SocketServer::~SocketServer()
{
    // 关闭服务器
    stopServer();
}


void SocketServer::startServer(int port)
{
    if (tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port:" << port;
    } else {
        qDebug() << "Failed to start server: " << tcpServer->errorString();
    }
}

void SocketServer::stopServer()
{
    if (tcpServer->isListening()) {
        tcpServer->close();
        qDebug() << "Server stopped.";
    }
}

void SocketServer::onNewConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::disconnected, this, &SocketServer::onClientDisConnection);
    connect(clientSocket, &QTcpSocket::readyRead, this, &SocketServer::onReadyRead);
}

void SocketServer::onClientDisConnection()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        qDebug() << "Client disconnected.";
        clientSocket->deleteLater();
    }
}

void SocketServer::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        QByteArray data = clientSocket->readAll();
        // 提取json信息
        QString jsonString = QString::fromUtf8(data);

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
        if (jsonDoc.isObject()) {
            QJsonObject jsObj = jsonDoc.object();

            QString type = jsObj.value("type").toString();
            QString imagePath = jsObj.value("image_path").toString();
            QString fromLanguage = jsObj.value("fromLanguage").toString();
            QString toLanguage = jsObj.value("toLanguage").toString();

            qDebug() << type;

            if (type == "ball") emit changeBallWallPaper(imagePath);
            else if (type  == "paper") emit changeQueryWallPaper(imagePath);
            else emit changeLanguages(fromLanguage, toLanguage);
        }
    }
}

// 还需要准备状态信息，也就是更换是否成功
