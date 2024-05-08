#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>
#include <QEvent>
#include <QPixmap>
#include <QPainter>

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *e) override;
signals:

};

#endif // MYPUSHBUTTON_H
