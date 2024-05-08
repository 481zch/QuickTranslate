#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <QObject>
// 与数据库相关的头文件
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>

class MyDataBase : public QObject
{
    Q_OBJECT
public:
    MyDataBase();
    // 创建一个共同路径的数据库
    void openOrCreateDatabase(); // 检查是否存在数据库，如果不存在的话就进行创建
    void checkAndCreateTables(); // 检查是否存在需要的表，如果不存在的话就进行创建
    // 插入数据，这里面需要插入的是：from、to、text、time
    void insertTranslateData(QString fromLanguage, QString toLanguage, QString translateText, QString translatedText, QDateTime curTime);
    QList<QString> getResData(); // 拿到用户定义的一些资源内容：壁纸、百度翻译的api中的内容
    void insertResText(); // 插入一些默认资源信息

private:
    QSqlDatabase db; // 数据库对象

signals:

};

#endif // MYDATABASE_H

