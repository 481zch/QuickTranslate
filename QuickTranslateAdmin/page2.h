#ifndef PAGE2_H
#define PAGE2_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <mydatabase.h>

namespace Ui {
class Page2;
}

class Page2 : public QWidget
{
    Q_OBJECT

public:
    bool successConnect;

public:
    explicit Page2(MyDataBase* db, QString ballWallPaper, QString backgroundWallPaper, QWidget *parent = nullptr);
    ~Page2();
    void setCurSearchImg(QString path);
    void setCurBallImg(QString path);

signals:
    void changeBallPaper(QString path);
    void changeWallPaper(QString path);

private:
    Ui::Page2 *ui;
    MyDataBase* db;
};

#endif // PAGE2_H
