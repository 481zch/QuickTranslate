#ifndef MYDATABASE_H
#define MYDATABASE_H

#define ERROR "DataBase Not Found"
#define SUCCESS "Check Success"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>

// 定义一个结构体
struct TranslationText{
    QString fromLanguage;
    QString toLanguage;
    QString translateText;
    QString translatedText;
    QDateTime time;
};

class MyDataBase : public QObject
{
    Q_OBJECT
public:
    MyDataBase();
    QString openOrCreateDatabase(); // 检查是否存在数据库
    QList<TranslationText> getRecordFromWord(); // 从给定的时间区间获取单词
    void getStartAndEndTime(); // 获取最早的时间和最晚的时间之间的记录
    bool uploadBallWallPaper(QString filePath); // 上传悬浮球壁纸
    bool uploadBackgroundWallPaper(QString filePath); // 上传翻译界面的壁纸
    bool chooseLanguageType(QString from, QString to); // 自定义翻译的语言种类
    QList<QString> getLanguageList(); // 获得目前支持的朗诵语言列表
    bool customApiRes(QString appid, QString appkey); // 用户传入自己的appid和appkey
    void setStartAndEnd(QDateTime s, QDateTime e){ start = s, end = e; } // 留下一个接口，可以让用户自己去定义显示的时间范围
    QList<QString> getResData(); // 获得当前使用的资源

    QDateTime start; // 起始区间
    QDateTime end; // 截至区间

private:
    QSqlDatabase db; // 数据库对象

    static std::unordered_map<QString, QString> conversionLanguageList;
};

#endif // MYDATABASE_H

// 需要的功能有：
// 1. 默认返回单词记录表中的所有内容，设置默认参数，可以返回用户定义区间的内容
// 2. 给予用户自定义悬浮球壁纸和的功能
// 3. 用户可以自己导出txt格式的文档，到不背单词、扇贝英语中上传自定义单词本进行背诵
// 4. 用户可以选定想要翻译的语言种类


// 提示：
// 需要到主应用界面的数据库增加一个检查功能，如果发现自定义表中的内容不可以访问的话，
// 我们就将表中对应的内容删除掉
