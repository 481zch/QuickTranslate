#include "page5.h"
#include "ui_page5.h"
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>

Page5::Page5(MyDataBase *db, QString fromLanguage, QString toLanguage, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Page5),
    db(db),
    conversionLanguageList({
        {"zh", "中文"}, {"en", "英语"}, {"jp", "日语"}, {"kor", "韩语"}, {"th", "泰语"}, {"vie", "越南语"}, {"ru", "俄语"},
        {"中文", "zh"}, {"英语", "en"}, {"日语", "jp"}, {"韩语", "kor"}, {"泰语", "th"}, {"越南语", "vie"}, {"俄语", "ru"}
    }),
    ttsToAppLanguageMap({
        {"zh", "zh"}, {"en", "en"}, {"ja", "jp"}, {"ko", "kor"}, {"th", "th"}, {"vi", "vie"}, {"ru", "ru"}
    }),
    supportedLanguages({"中文", "英语", "日语", "韩语", "泰语", "越南语", "俄语"})
{
    ui->setupUi(this);
    ui->lineEditFrom->setReadOnly(true);
    ui->lineEditTo->setReadOnly(true);
    ui->lineEditFrom->setText(conversionLanguageList[fromLanguage]);
    ui->lineEditTo->setText(conversionLanguageList[toLanguage]);

    initializeLanguageList();

    connect(ui->upLoad, &QPushButton::clicked, this, &Page5::upLoadLanguage);
    connect(ui->listWidget, &QListWidget::itemChanged, this, &Page5::onListItemChanged);
}

Page5::~Page5()
{
    delete ui;
}

void Page5::initializeLanguageList()
{
    QList<QString> ttsSupportedLanguages = db->getLanguageList();
    QList<QString> speechLanguage;
    for (const auto& lang : ttsSupportedLanguages) {
        QString appLang = ttsToAppLanguageMap[lang];
        if (!appLang.isEmpty()) {
            speechLanguage.append(appLang);
        }
    }

    for (const auto& language : supportedLanguages) {
        QString displayText = language + (speechLanguage.contains(conversionLanguageList[language]) ? " (可发音)" : "");
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        ui->listWidget->addItem(item);
    }
}

void Page5::onListItemChanged()
{
    // 计算已勾选项的数量
    int checkedCount = 0;
    for(int i = 0; i < ui->listWidget->count(); ++i) {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked) {
            ++checkedCount;
        }
    }

    // 如果勾选项超过两个，则显示提示消息
    if (checkedCount > 2) {
        QMessageBox::warning(this, "选择限制", "您只能选择两种语言，请取消一些选择。");
    }
}

void Page5::upLoadLanguage() {
    int checkedCount = 0;
    QStringList selectedLanguages;

    // 提取已选择的语言
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            QString languageFullName = item->text().split(" (").first(); // 移除" (可发音)"
            QString languageShortName = conversionLanguageList[languageFullName];
            selectedLanguages.append(languageShortName);
            checkedCount++;
        }
    }

    // 检查是否正好选择了两种语言
    if (checkedCount != 2) {
        QMessageBox::warning(this, "选择错误", "您必须选择两种语言。");
        return;
    }

    emit changeLanguage(selectedLanguages.at(0), selectedLanguages.at(1));

    // 进行语言上传操作
    bool uploadSuccess = db->chooseLanguageType(selectedLanguages.at(0), selectedLanguages.at(1));
    if (uploadSuccess) {
        ui->lineEditFrom->setText(conversionLanguageList[selectedLanguages.at(0)]);
        ui->lineEditTo->setText(conversionLanguageList[selectedLanguages.at(1)]);
    } else {
        QMessageBox::critical(this, "操作失败", "更新失败，请重新尝试。");
    }

    if (uploadSuccess && !successConnect) {
        QMessageBox::information(this, "操作成功", "语言选择成功更新，重启使用。");
    }
}

