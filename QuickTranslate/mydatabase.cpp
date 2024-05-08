#include "mydatabase.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>

MyDataBase::MyDataBase()
{
    // 每一次打开数据库都进行查询是否完成了建立工作
    openOrCreateDatabase();
    checkAndCreateTables();
    insertResText();
}

void MyDataBase::openOrCreateDatabase()
{
    // 确定共同的应用程序数据库
    QCoreApplication::setOrganizationName("QuickTranslate");
    QCoreApplication::setApplicationName("data");

    // 获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    // 确保目录存在
    QDir dir(dataPath);
    if (!dir.exists(dataPath)) {
        // 不存在的话就创建一个对应的目录
        dir.mkpath(dataPath);
    }

    // 构建数据库文件的完整路径
    QString databasePath = dir.filePath("quickTranslate.db");

    // 设置SQLite数据库
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    this->db.setDatabaseName(databasePath);

    // 尝试打开数据库
    if (!this->db.open()) {
        qDebug() << "数据库打开失败：" << this->db.lastError().text();
    }
}

void MyDataBase::checkAndCreateTables()
{
    QSqlQuery query(this->db);
    QStringList existingTables = db.tables();
    QStringList tablesToCheck = {"defaultRes", "wordRecord", "customizeRes"};

    // 依次创建每个表
    foreach (const QString &table, tablesToCheck) {
        if (!existingTables.contains(table, Qt::CaseInsensitive)) {
            if (table == "defaultRes") {
                // 创建defaultRes
                bool success = query.exec("CREATE TABLE defaultRes ("
                                          "ballWallPaper TEXT, "
                                          "backgroundWallPaper TEXT, "
                                          "fromLanguage TEXT, "
                                          "toLanguage TEXT, "
                                          "appID TEXT, "
                                          "appKey TEXT)");
                if (!success) {
                    qDebug() << "Cannot create defaultRes:" << query.lastError();
                }
            } else if (table == "wordRecord") {
                // 创建wordRecord
                bool success = query.exec("CREATE TABLE wordRecord ("
                                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                          "fromLanguage TEXT, "
                                          "toLanguage TEXT, "
                                          "translateText TEXT, "
                                          "translatedText TEXT, "
                                          "searchTime TIME)");
                if (!success) {
                    qDebug() << "Cannot create wordRecord:" << query.lastError();
                }
            } else if (table == "customizeRes") {
                // 创建customizeRes，如果在后面的查询过程中发现某一条记录消失了，那么就将那一条记录中的内容使用默认信息
                bool success = query.exec("CREATE TABLE customizeRes ("
                                          "ballWallPaper TEXT, "
                                          "backgroundWallPaper TEXT, "
                                          "fromLanguage TEXT, "
                                          "toLanguage TEXT, "
                                          "appID TEXT, "
                                          "appKey TEXT)");
                if (!success) {
                    qDebug() << "Cannot create customizeRes:" << query.lastError();
                }
            }
        }
    }
}


void MyDataBase::insertResText()
{
    // 检查数据库表是否已填充
    QSqlQuery query;
    if (query.exec("SELECT COUNT(*) FROM defaultRes")) {
        if (query.next()) {
            int count = query.value(0).toInt();
            if (count > 0) {
                return;
            }
        }
    }

    // 复制资源到应用程序目录
    QString ballWallPaperPath = qApp->applicationDirPath() + "/ball.png";
    QString backgroundWallPaperPath = qApp->applicationDirPath() + "/background.png";
    QFile::copy(":/rec/ball.png", ballWallPaperPath);
    QFile::copy(":/rec/background.png", backgroundWallPaperPath);

    // 确保路径使用的是数据库能正确处理的格式
    ballWallPaperPath.replace("'", "''");
    backgroundWallPaperPath.replace("'", "''");
    // 替换为‘/’模式
    ballWallPaperPath.replace("\\", "/");
    backgroundWallPaperPath.replace("\\", "/");

    // 插入路径到数据库
    bool success = query.exec(QString("INSERT INTO defaultRes (ballWallPaper, backgroundWallPaper, fromLanguage, toLanguage, appID, appKey) "
                                      "VALUES ('%1', '%2', 'zh', 'en', '20240312001991249', 'RDRk2HXsJA8QuFGhZFYW')")
                                  .arg(ballWallPaperPath)
                                  .arg(backgroundWallPaperPath));
    if (!success) {
        qDebug() << "插入默认资源数据时失败：" << query.lastError().text();
    }
}


void MyDataBase::insertTranslateData(QString fromLanguage, QString toLanguage, QString translateText, QString translatedText, QDateTime curTime)
{
    QSqlQuery query;
    query.prepare("INSERT INTO wordRecord (fromLanguage, toLanguage, translateText, translatedText, searchTime) "
                  "VALUES (?, ?, ?, ?, ?)");

    // 绑定值到预处理语句
    query.addBindValue(fromLanguage);
    query.addBindValue(toLanguage);
    query.addBindValue(translateText);
    query.addBindValue(translatedText);
    // 将QDate转换为适用于SQL的字符串格式
    query.addBindValue(curTime.toString("yyyy-MM-dd HH:mm:ss"));

    // 执行查询
    bool success = query.exec();
    if (!success) {
        // 如果插入失败，打印错误信息
        qDebug() << "Insert failed: " << query.lastError().text();
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

    return resDataList;
}
