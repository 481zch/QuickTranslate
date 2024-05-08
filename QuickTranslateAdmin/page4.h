#ifndef PAGE4_H
#define PAGE4_H

#include <QWidget>
#include <mydatabase.h>
#include <QDateTime>


namespace Ui {
class Page4;
}

class Page4 : public QWidget
{
    Q_OBJECT

public:
    explicit Page4(MyDataBase* db, QWidget *parent = nullptr);
    void getTxt();
    ~Page4();

private:
    Ui::Page4 *ui;
    MyDataBase* db;
    QDateTime start;
    QDateTime end;
};

#endif // PAGE4_H
