#ifndef FLOATINGBALL_H
#define FLOATINGBALL_H

#include <QMainWindow>
#include <QEvent>
#include <QPoint>
#include <QPropertyAnimation>
#include "queryinterface.h"
#include "mydatabase.h"
#include <QSystemTrayIcon>
#include "network.h"
#include "getlanguage.h"
#include "socketserver.h"

class FloatingBall : public QMainWindow
{
    Q_OBJECT

public:
    FloatingBall(QWidget *parent = nullptr);
    ~FloatingBall();
    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent *event) override; // 重写鼠标事件，为了进行界面的转换
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void zoom1(); // 设置动画效果，返回侧边缘

public slots:
signals:
    void start(int x, int y, MyDataBase* db);

private:
    QSystemTrayIcon *trayIcon;      // 系统托盘图标
    QPoint dragPosition;            // 拖拽的位置
    bool isDragging;                // 是否拖拽
    QueryInterface *query;          // 监听界面
    MyDataBase *db;                 // 自定义数据库的对象
    QList<QString> curRes;          // 当前的资源列表
    SocketServer *server;           // 定义响应服务器
};
#endif // FLOATINGBALL_H
