#include "page4.h"
#include "ui_page4.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QFileDialog>
#include <QMessageBox>

Page4::Page4(MyDataBase* db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page4),
    db(db)
{
    ui->setupUi(this);

    // 超链接文本标签
    // 不背单词
    ui->href1->setText("<a href=\"https://bbdc.cn\"><img src=':/res/bubeidanci.png' width='30' height='30'/></a>");
    ui->href1->setTextFormat(Qt::RichText);
    ui->href1->setOpenExternalLinks(true);

    // 扇贝英语
    ui->href2->setText("<a href=\"https://web.shanbay.com/web/main/index\"><img src=':/res/shanbei.png'  width='30' height='30'/></a>");
    ui->href2->setTextFormat(Qt::RichText);
    ui->href2->setOpenExternalLinks(true);

    // 设置默认的起始和截至时间
    ui->dateStart->setDate(db->start.date());
    ui->dateEnd->setDate(db->end.date());

    // 设置信号槽：当点击按钮后，会以时间节点选定时间范围下载数据至保存位置：只会挑选出来对应的英文单词
    connect(ui->download, &QPushButton::clicked, this, [=](){
        QDateTime startDate = QDateTime(ui->dateStart->date(), QTime(0, 0, 0)).addSecs(-1);
        QDateTime endDate = QDateTime(ui->dateEnd->date(), QTime(23, 59, 59));

        db->setStartAndEnd(startDate, endDate);
        getTxt();
    });
}

void Page4::getTxt()
{
    QList<TranslationText> records = db->getRecordFromWord();
    QStringList englishWords;

    // 筛选出所有英文单词
    for (const TranslationText &record : records) {
        if (record.fromLanguage == "en" && !record.translateText.trimmed().contains(" ")) {
            englishWords << record.translateText.trimmed();
        } else if (record.toLanguage == "en" && !record.translatedText.trimmed().contains(" ")) {
            englishWords << record.translatedText.trimmed();
        }
    }

    // 去重
    englishWords.removeDuplicates();

    // 让用户选择文件保存路径
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(), tr("Text Files (*.txt)"));
    if (filePath.isEmpty()) {
        return; // 用户取消了操作
    }

    // 写入文件
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        for (const QString &word : englishWords) {
            out << word << '\n';
        }
        file.close();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Could not save the file."));
    }
}

Page4::~Page4()
{
    delete ui;
}
