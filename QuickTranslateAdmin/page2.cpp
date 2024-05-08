#include "page2.h"
#include "ui_page2.h"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>

Page2::Page2(MyDataBase* db, QString ballWallPaper, QString backgroundWallPaper, QWidget *parent) :
    ui(new Ui::Page2), QWidget(parent)
{
    ui->setupUi(this);
    this->db = db;

    QString ballPaper = ballWallPaper;
    QString searchPaper = backgroundWallPaper;
    setCurSearchImg(searchPaper);
    setCurBallImg(ballPaper);


    connect(ui->uploadSearch, &QPushButton::clicked, this, [=](){
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Images (*.png *.jpg *.jpeg)"));
        if (!fileName.isEmpty()) {
            fileName.replace("\\", "/");
            emit changeWallPaper(fileName);
            // 检查是否已经建立了连接，如果建立了连接，直接告诉已经更换完毕
            bool flag = db->uploadBackgroundWallPaper(fileName);
            if (flag && !successConnect) {
                QMessageBox::information(this, tr("上传成功"),
                                         tr("请重新打开QuickTranslate软件查看。"),
                                         QMessageBox::Ok);
            }
            setCurSearchImg(fileName);
        }
    });

    connect(ui->uploadBall, &QPushButton::clicked, this, [=](){
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Images (*.png *.jpg *.jpeg)"));
        if (!fileName.isEmpty()) {
            fileName.replace("\\", "/");
            emit changeBallPaper(fileName);
            bool flag = db->uploadBallWallPaper(fileName);
            if (flag && !successConnect) {
                QMessageBox::information(this, tr("上传成功"),
                                         tr("请重新打开QuickTranslate软件查看。"),
                                         QMessageBox::Ok);
            }
            setCurBallImg(fileName);
        }
    });
}

Page2::~Page2()
{
    delete ui;
}

void Page2::setCurSearchImg(QString path)
{
    QPixmap pix;
    if(pix.load(path)) {
        ui->imgSearch->setPixmap(pix.scaled(ui->imgSearch->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void Page2::setCurBallImg(QString path)
{
    QPixmap pix;
    if(pix.load(path)) {
        ui->imgBall->setPixmap(pix.scaled(ui->imgBall->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
