#include "page1.h"
#include "ui_page1.h"

Page1::Page1(MyDataBase* db, QWidget *parent): ui(new Ui::Page1), QWidget(parent)
{
    ui->setupUi(this);
    // 禁止直接写入
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->db = db;

    displayRecords();

    // 默认区间范围
    ui->dateStart->setDate(db->start.date());
    ui->dateEnd->setDate(db->end.date());

    connect(ui->queryButton, &QPushButton::clicked, this, [=](){
        QDateTime startDate = QDateTime(ui->dateStart->date(), QTime(0, 0, 0)).addSecs(-1);
        QDateTime endDate = QDateTime(ui->dateEnd->date(), QTime(23, 59, 59));

        db->setStartAndEnd(startDate, endDate);
        displayRecords();
    });
}

Page1::~Page1()
{
    delete ui;
}

void Page1::displayRecords()
{
    // 从数据库获取数据记录
    QList<TranslationText> records = db->getRecordFromWord();

    // 设置表格的行数为数据记录的数量
    ui->table->setRowCount(records.size());
    ui->table->setColumnCount(4);

    // 设置表头标签
    QStringList headers;
    headers << "查询语言" << "翻译语言" << "查询文本" << "翻译文本";
    ui->table->setHorizontalHeaderLabels(headers);
    ui->table->verticalHeader()->setVisible(false);
    ui->table->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < records.size(); ++i) {
        const TranslationText& record = records.at(i);
        QTableWidgetItem *fromLanguageItem = new QTableWidgetItem(record.fromLanguage);
        QTableWidgetItem *toLanguageItem = new QTableWidgetItem(record.toLanguage);
        QTableWidgetItem *translateTextItem = new QTableWidgetItem(record.translateText);
        QTableWidgetItem *translatedTextItem = new QTableWidgetItem(record.translatedText);

        // 设置每一行的单元格数据
        ui->table->setItem(i, 0, fromLanguageItem);
        ui->table->setItem(i, 1, toLanguageItem);
        ui->table->setItem(i, 2, translateTextItem);
        ui->table->setItem(i, 3, translatedTextItem);
    }
}
