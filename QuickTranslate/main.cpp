#include "floatingball.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/rec/cat1.ico"));
    FloatingBall w;
    w.show();
    return a.exec();
}
