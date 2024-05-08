#ifndef PAGE1_H
#define PAGE1_H

#include <QWidget>
#include <QTableWidget>
#include "mydatabase.h"

namespace Ui {
class Page1;
}

class Page1 : public QWidget
{
    Q_OBJECT

public:
    explicit Page1(MyDataBase *db, QWidget *parent = nullptr);
    ~Page1();
    void displayRecords();

private:
    Ui::Page1 *ui;
    MyDataBase* db;
};

#endif // PAGE1_H
