#ifndef PAGE5_H
#define PAGE5_H

#include <QWidget>
#include "mydatabase.h"

namespace Ui {
class Page5;
}

class Page5 : public QWidget
{
    Q_OBJECT

public:
    bool successConnect;
public:
    explicit Page5(MyDataBase *db, QString fromLanguage, QString toLanguage, QWidget *parent = nullptr);
    ~Page5();

signals:
    void changeLanguage(QString from, QString to);

private slots: // 将槽函数定义为私有

    void upLoadLanguage();
    void onListItemChanged();

private:
    void initializeLanguageList(); // 添加一个初始化语言列表的函数
    Ui::Page5 *ui;
    MyDataBase *db;
    std::unordered_map<QString, QString> conversionLanguageList;
    std::unordered_map<QString, QString> ttsToAppLanguageMap;
    QStringList supportedLanguages;
};

#endif // PAGE5_H
