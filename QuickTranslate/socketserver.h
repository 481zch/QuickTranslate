#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

class SocketServer: public QObject
{
    Q_OBJECT
public:
    explicit SocketServer(QObject *parent = nullptr);
    ~SocketServer();

    void startServer(int port);
    void stopServer();

signals:
    void changeBallWallPaper(QString s);
    void changeQueryWallPaper(QString s);
    void changeLanguages(QString from, QString to);

public slots:
    void onNewConnection();
    void onClientDisConnection();
    void onReadyRead();

private:
    QTcpServer *tcpServer;
};

#endif // SOCKETSERVER_H
