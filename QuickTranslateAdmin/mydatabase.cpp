#include "mydatabase.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QtTextToSpeech>
#include <QDebug>

MyDataBase::MyDataBase()
{
    // 检查是否存在目标数据库
    openOrCreateDatabase();
    // 获取到最早和最晚的默认时间值
    getStartAndEndTime();
}

QString MyDataBase::openOrCreateDatabase()
{
    // 确定共同的应用程序数据库
    QCoreApplication::setOrganizationName("QuickTranslate");
    QCoreApplication::setApplicationName("data");

    // 获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        qDebug() << "数据目录不存在：" << dataPath;
        return ERROR;
    }

    // 构建数据库文件的完整路径
    QString databasePath = dir.filePath("quickTranslate.db");
    if (!QFile::exists(databasePath)) {
        qDebug() << "数据库文件不存在：" << databasePath;
        return ERROR;
    }

    // 设置SQLite数据库
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    this->db.setDatabaseName(databasePath);

    // 尝试打开数据库
    if (!this->db.open()) {
        qDebug() << "数据库打开失败：" << this->db.lastError().text();
        return ERROR;
    } else {
        qDebug() << "数据库已成功打开。路径：" << databasePath;
        return SUCCESS;
    }
}


// 从给定的时间区间获取单词记录
QList<TranslationText> MyDataBase::getRecordFromWord()
{
    QList<TranslationText> records;

    // SQL查询，获取指定时间区间内的记录
    QSqlQuery query;
    QString queryString = QString("SELECT fromLanguage, toLanguage, translateText, translatedText, searchTime "
                                  "FROM wordRecord "
                                  "WHERE searchTime BETWEEN '%1' AND '%2'"
                                  "ORDER BY searchTime DESC")
                              .arg(start.toString("yyyy-MM-dd HH:mm:ss"))
                              .arg(end.toString("yyyy-MM-dd HH:mm:ss"));

    if (!query.exec(queryString)) {
        qDebug() << "Failed to query records:" << query.lastError();
        return records;
    }

    // 遍历查询结果
    while (query.next()) {
        TranslationText record;
        record.fromLanguage = query.value(0).toString();
        record.toLanguage = query.value(1).toString();
        record.translateText = query.value(2).toString();
        record.translatedText = query.value(3).toString();
        record.time = query.value(4).toDateTime();
        records.append(record);
    }

    return records;
}

void MyDataBase::getStartAndEndTime()
{
    QSqlQuery query;
    // 查询最早的记录时间
    query.exec("SELECT searchTime FROM wordRecord ORDER BY searchTime ASC LIMIT 1");
    if (query.next()) {
        // 转化为QDateTime
        start = QDateTime::fromString(query.value(0).toString(), Qt::ISODate);
    } else {
        qDebug() << "Failed to find the earliest time:" << query.lastError();
        return;
    }

    // 查询最晚的记录时间
    query.exec("SELECT searchTime FROM wordRecord ORDER BY searchTime DESC LIMIT 1");
    if (query.next()) {
        end = QDateTime::fromString(query.value(0).toString(), Qt::ISODate);
    } else {
        qDebug() << "Failed to find the latest time:" << query.lastError();
        return;
    }

    qDebug() << "Start Time:" << start << "\nEnd Time:" << end;
}

bool MyDataBase::uploadBallWallPaper(QString filePath)
{
    // 检查表中是否有数据
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM customizeRes");
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        // 表中已经有数据，更新ballWallPaper字段
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE customizeRes SET ballWallPaper = ?");
        updateQuery.addBindValue(filePath);
        return updateQuery.exec();
    } else {
        // 表中没有数据，插入新记录
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO customizeRes (ballWallPaper) VALUES (?)");
        insertQuery.addBindValue(filePath);
        return insertQuery.exec();
    }
}


bool MyDataBase::uploadBackgroundWallPaper(QString filePath)
{
    // 检查表中是否有数据
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM customizeRes");
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        // 表中已经有数据，更新backgroundWallPaper字段
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE customizeRes SET backgroundWallPaper = ?");
        updateQuery.addBindValue(filePath);
        return updateQuery.exec();
    } else {
        // 表中没有数据，插入新记录
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO customizeRes (backgroundWallPaper) VALUES (?)");
        insertQuery.addBindValue(filePath);
        return insertQuery.exec();
    }
}

bool MyDataBase::chooseLanguageType(QString from, QString to)
{
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM customizeRes");
    // 如果执行失败或者没有结果集或者其中的内容为空，则进行更新操作
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO customizeRes (fromLanguage, toLanguage) VALUES (?, ?)");
        insertQuery.addBindValue(from);
        insertQuery.addBindValue(to);
        return insertQuery.exec();
    }
    else
    {
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE customizeRes SET fromLanguage = ?, toLanguage = ? WHERE rowid = (SELECT rowid FROM customizeRes LIMIT 1)");
        updateQuery.addBindValue(from);
        updateQuery.addBindValue(to);
        return updateQuery.exec();
    }
}

QList<QString> MyDataBase::getLanguageList()
{
    QList<QString> languageList;
    QTextToSpeech *speech = new QTextToSpeech(this);
    QList<QLocale> locales = speech->availableLocales();

    foreach (const QLocale &locale, locales) {
        QString languageCode = locale.name();
        languageList.append(languageCode.sliced(0, 2));
    }

    delete speech;
    return languageList;
}

bool customApiRes(QString appid, QString appkey)
{
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM customizeRes");
    // 如果执行失败或者没有结果集或者其中的内容为空，则进行更新操作
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO customizeRes (appID, appKey) VALUES (?, ?)");
        insertQuery.addBindValue(appid);
        insertQuery.addBindValue(appkey);
        return insertQuery.exec();
    }
    else
    {
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE customizeRes SET appID = ?, appKey = ? WHERE rowid = (SELECT rowid FROM customizeRes LIMIT 1)");
        updateQuery.addBindValue(appid);
        updateQuery.addBindValue(appkey);
        return updateQuery.exec();
    }
}

QList<QString> MyDataBase::getResData()
{
    QSqlQuery query;
        // 初始化一个有四个元素的列表
    QList<QString> resDataList(6);

    // 首先尝试从自定义资源表中获取数据
    if (query.exec("SELECT ballWallPaper, backgroundWallPaper, fromLanguage, toLanguage, appID, appKey FROM customizeRes LIMIT 1") && query.next()) {
        for (int i = 0; i < 6; ++i) {
            if (!query.value(i).toString().isEmpty()) {
                // 如果自定义资源表中的数据项不为空，则使用该项
                resDataList[i] = query.value(i).toString();
            }
        }
    }

    // 然后从默认资源表中补全缺失的数据
    if (query.exec("SELECT ballWallPaper, backgroundWallPaper, fromLanguage, toLanguage, appID, appKey FROM defaultRes LIMIT 1") && query.next()) {
        for (int i = 0; i < 6; ++i) {
            if (resDataList[i].isEmpty()) {
                // 如果自定义资源表中的相应数据项为空，则使用默认资源表中的数据
                resDataList[i] = query.value(i).toString();
            }
        }
    }

    // 替换简写
//    resDataList[2] = conversionLanguageList[resDataList[2]];
//    resDataList[3] = conversionLanguageList[resDataList[3]];

    return resDataList;
}
