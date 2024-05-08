#ifndef QUERYINTERFACE_H
#define QUERYINTERFACE_H

#include <QMainWindow>
#include "mypushbutton.h"
#include "mylineedit.h"
#include <QPoint>
#include <QTextToSpeech>
#include <QMouseEvent>
#include <QEvent>
#include <QPixmap>
#include "getlanguage.h"
#include "network.h"
#include "mydatabase.h"

class QueryInterface : public QMainWindow
{
    Q_OBJECT
public:
    explicit QueryInterface(QPoint pos, MyDataBase* db, QWidget *parent = nullptr);
    void setLocation(QPoint cur);
    ~QueryInterface();

protected:
    void paintEvent(QPaintEvent *e) override;
    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void updateScaledPixmap();


public slots:
    void handleLanguageDetection(QString src);
    void handleTranslationFinished(QString result);
    void changePaper(QString path);
    void changeLanguage(QString from, QString to);

private:
    void setupUi();
    void setupConnections();
    void initiateTranslation(const QString &text);

    QPixmap scaledPix;
    QTextToSpeech *tts;
    MyPushButton *play;
    MyLineEdit *input;
    MyLineEdit *output;
    GetLanguage *getLanguage;
    NetWork *network;
    MyDataBase *db;
    QString fromLanguage;
    QString toLanguage;
    QPoint pos;
    std::pair<QString, QString> setLanguage;
    QList<QString> curRes;
    std::unordered_map<QString, QString> ttsToAppLanguageMap;

signals:
    void changeFocus();
    void getLanguageTypeFinish();
};

#endif // QUERYINTERFACE_H
