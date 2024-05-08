#include "mylineedit.h"


MyLineEdit::MyLineEdit(QWidget *parent):QLineEdit(parent)
{
    this->setStyleSheet("QLineEdit { background-color: rgba(255, 255, 255, 127); border-radius: 10px; }");
}


