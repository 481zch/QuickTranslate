#include "page3.h"
#include "ui_page3.h"

Page3::Page3(MyDataBase* db, QWidget *parent) :
    ui(new Ui::Page3),
    db(db),
    QWidget(parent)
{
    ui->setupUi(this);
    connect(ui->determineButton, &QPushButton::clicked, this, &Page3::handleDetermineButtonClick);
}

void Page3::handleDetermineButtonClick() {
    if (checkInput()) {
        TestApi *testApi = new TestApi(ui->inputAppid->text(), ui->inputAppkey->text());
        connect(testApi, &TestApi::testSuccess, this, &Page3::onTestApiSuccess);
        testApi->testNet();
    }
}

void Page3::onTestApiSuccess(bool isValid) {
    if (isValid) {
        QMessageBox::information(this, tr("成功"), tr("提供密匙有效，已成功上传，重启"));
    } else {
        QMessageBox::warning(this, tr("失败"), tr("密匙无效，请重新上传"));
    }
}

bool Page3::checkInput() {
    if (ui->inputAppid->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("未输入appid"), tr("请输入appid"));
        return false;
    }

    if (ui->inputAppkey->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("未输入appkey"), tr("请输入appkey"));
        return false;
    }

    return true;
}

