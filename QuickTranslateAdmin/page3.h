#ifndef PAGE3_H
#define PAGE3_H

#include <QWidget>
#include "mydatabase.h"
#include "testapi.h"
#include <QMessageBox>

namespace Ui {
class Page3;
}

class Page3 : public QWidget
{
    Q_OBJECT

public:
    explicit Page3(MyDataBase* db, QWidget *parent = nullptr);
    void handleDetermineButtonClick();
    bool checkInput();

public slots:
    void onTestApiSuccess(bool isValid);

private:
    Ui::Page3 *ui;
    MyDataBase* db;
};

#endif // PAGE3_H
