#include "mypushbutton.h"

MyPushButton::MyPushButton(QWidget *parent) : QPushButton(parent)
{
}

void MyPushButton::paintEvent(QPaintEvent *e) {
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 检查按钮是否被按下，并设置相应的颜色
    QColor bgColor = isDown() ? QColor(200, 200, 200, 127) : QColor(255, 255, 255, 127); // 点击时变为较暗的颜色
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen); // 无边框

    QRect rect = this->rect();
    int borderRadius = 8;
    painter.drawRoundedRect(rect, borderRadius, borderRadius);

    // 绘制按钮文本
    painter.setPen(Qt::black); // 设置文本颜色
    painter.drawText(rect, Qt::AlignCenter, text());
}

