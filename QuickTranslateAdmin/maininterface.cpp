#include "maininterface.h"
#include "ui_maininterface.h"
#include <QColor>
#include <QThread>
#include <iostream>


MainInterface::MainInterface(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint)
    , ui(new Ui::MainInterface), client(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/mushroom.ico"));
    this->db = new MyDataBase();
    QList<QString> curRes = db->getResData();

    // 建立连接事件
    connect(ui->exit, &QPushButton::clicked, this, [=](){
        this->close();
    });

    connect(ui->ignore, &QPushButton::clicked, this, [=](){
        this->showMinimized();
    });

    // 客户端连接，不成功的话子线程中间断的发送连接请求
    client = new SocketClient(this);
    client->connectToServer("127.0.0.1", 12345);

    // 创建页面和点击事件
    Page1 *page1 = new Page1(db, this);
    ui->stackedWidget->addWidget(page1);

    Page2 *page2 = new Page2(db, curRes[0], curRes[1], this);
    ui->stackedWidget->addWidget(page2);

    Page3 *page3 = new Page3(db, this);
    ui->stackedWidget->addWidget(page3);

    Page4 *page4 = new Page4(db, this);
    ui->stackedWidget->addWidget(page4);

    Page5 *page5 = new Page5(db, curRes[2], curRes[3], this);
    ui->stackedWidget->addWidget(page5);

    // 默认显示第一页
    ui->stackedWidget->setCurrentWidget(page1);

    connect(ui->button1, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(page1);
    });

    connect(ui->button2, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(page2);
    });

    connect(ui->button3, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(page3);
    });

    connect(ui->button4, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(page4);
    });

    connect(ui->button5, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentWidget(page5);
    });

    // 传递连接状态
    connect(client, &SocketClient::connectSucess, page2, [=]{
        page2->successConnect = true;
    });

    connect(client, &SocketClient::connectSucess, page5, [=]{
        page5->successConnect = true;
    });

    // 发送信息
    connect(page2, &Page2::changeBallPaper, client, [=](QString imagePath){
        QJsonObject jsonObject;
        jsonObject["type"] = "ball";
        jsonObject["image_path"] = imagePath;
        QJsonDocument doc(jsonObject);
        client->sendMessage(doc.toJson(QJsonDocument::Compact));
    });

    connect(page2, &Page2::changeWallPaper, client, [=](QString imagePath){
        QJsonObject jsonObject;
        jsonObject["type"] = "paper";
        jsonObject["image_path"] = imagePath;
        QJsonDocument doc(jsonObject);
        client->sendMessage(doc.toJson(QJsonDocument::Compact));
    });

    connect(page5, &Page5::changeLanguage, client, [=](QString fromLanguage, QString toLanguage){
        QJsonObject jsonObject;
        jsonObject["type"] = "changeLanguage";
        jsonObject["fromLanguage"] = fromLanguage;
        jsonObject["toLanguage"] = toLanguage;
        QJsonDocument doc(jsonObject);
        client->sendMessage(doc.toJson(QJsonDocument::Compact));
    });
}

MainInterface::~MainInterface()
{
    delete ui;
}

void MainInterface::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    // Set a white semi-transparent brush.
    QColor semiTransparentColor(255, 255, 255, 210);
    painter.setBrush(semiTransparentColor);

    int width = 700; // 宽度
    int height = 450; // 高度
    int radius = 20; // 圆角半径

    // 绘制带有圆角的矩形
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿
    painter.drawRoundedRect(0, 0, width, height, radius, radius);

    // 设置控件的固定大小
    setFixedSize(width, height);
}

void MainInterface::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 记录鼠标按下时的全局位置和窗口位置
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainInterface::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // 移动窗口到当前鼠标位置
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void MainInterface::mouseReleaseEvent(QMouseEvent *event)
{
    // 鼠标释放时清除记录的位置
    Q_UNUSED(event);
    m_dragPosition = QPoint();
}
