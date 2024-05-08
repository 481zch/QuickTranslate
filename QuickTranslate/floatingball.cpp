#include "floatingball.h"
#include <QPixmap>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QTimer>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include "screenwidget.h"

FloatingBall::FloatingBall(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    QElapsedTimer timer;
    timer.start();

    setAttribute(Qt::WA_TranslucentBackground);
    // 隐藏状态栏图标
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool);
    // 设置悬浮球初始的停留位置
    setGeometry(QRect(QGuiApplication::primaryScreen()->geometry().width() - 50, 200, 50, 50));

    // 初始化数据库对象
    db = new MyDataBase;
    curRes = db->getResData();
    query = new QueryInterface(QPoint(this->x(), this->y()), db);

    // 初始化服务器
    server = new SocketServer(this);
    connect(server, &SocketServer::changeBallWallPaper, this, [=](QString path){
        curRes[0] = path;
        this->update();
    });

    connect(server, &SocketServer::changeQueryWallPaper, query, &QueryInterface::changePaper);

    connect(server, &SocketServer::changeLanguages, query, &QueryInterface::changeLanguage);

    // 托盘图标以及对应的菜单
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/rec/cat.png"));
    trayIcon->setVisible(true);

    QMenu *menu = new QMenu;
    QAction *quit;

    quit = new QAction("退出");
    connect(quit, &QAction::triggered, [&](){
        QCoreApplication::quit();
    });

    menu->addAction(quit);

    trayIcon->setContextMenu(menu);

    // 显示托盘图标
    trayIcon->show();

    qint64 elapsedTime = timer.elapsed();
    qDebug() << "Initialization completed in" << elapsedTime << "milliseconds";
}


FloatingBall::~FloatingBall(){}

void FloatingBall::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    QPixmap pix;
    pix.load(curRes[0]);
    pix = pix.scaled(pix.width() * 0.1, pix.height() * 0.1);
    painter.drawPixmap(0, 0, this->width(), this->height(), pix);
    setFixedSize(pix.width(), pix.height());
}

// 重写鼠标点击事件
void FloatingBall::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        isDragging = true;
        event->accept();
    }
    else if (event->button() == Qt::RightButton) {
        // 进行截图事件
        ScreenWidget *screenshotTool = ScreenWidget::Instance();
        screenshotTool->showFullScreen();
        event->accept(); // 确保事件被处理
    }
}

// 重写鼠标移动事件
void FloatingBall::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && isDragging) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

// 重写鼠标释放事件
void FloatingBall::mouseReleaseEvent(QMouseEvent *event)
{
    // 如果鼠标按着左键拖拽
    if (event->button() == Qt::LeftButton && isDragging) {
        isDragging = false;
        // 在释放后，进入查询界面
        query->setLocation(QPoint(this->x(), this->y()));
        this->hide();
        query->show();

        // 槽函数：当检测到焦点变换信号后，恢复成悬浮球的样式
        connect(query, &QueryInterface::changeFocus, this, [=](){
            query->hide();
            this->show();
            this->zoom1();
        });

        event->accept();
    }
}

// 动画效果：落回屏幕右边缘的碰撞
void FloatingBall::zoom1()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    int screenWidth = QGuiApplication::primaryScreen()->geometry().width();
    int newX = screenWidth - this->width(); // 屏幕宽度减去窗口宽度，计算新的X位置

    // 设置动画的持续时间
    animation->setDuration(1500);
    // 设置起始位置
    animation->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
    // 设置结束位置
    animation->setEndValue(QRect(newX, this->y(), this->width(), this->height()));
    // 设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);

    // 执行动画
    animation->start();
}
