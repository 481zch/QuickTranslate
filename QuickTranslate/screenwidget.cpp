#include "screenwidget.h"
#include <QMutex>
#include <QApplication>
#include <QPainter>
#include <QFileDialog>
#include <QEvent>
#include <QDateTime>
#include <QStringList>
#include <QToolBar>
#include <QToolButton>
#include <QClipboard>

#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QScreen>
#endif

#define STRDATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

Screen::Screen(QSize size)
{
    maxWidth = size.width();
    maxHeight = size.height();

    // 初始化为无效位置
    startPos = QPoint(-1, -1);
    endPos = startPos;
    leftUpPos = startPos;
    rightDownPos = startPos;
    status = SELECT;
}

int Screen::width()
{
    return maxWidth;
}

int Screen::height()
{
    return maxHeight;
}

Screen::STATUS Screen::getStatus()
{
    return status;
}

void Screen::setStatus(STATUS status)
{
    this->status = status;
}

void Screen::setEnd(QPoint pos)
{
    endPos = pos;
    leftUpPos = startPos;
    rightDownPos = endPos;
    cmpPoint(leftUpPos, rightDownPos);
}

void Screen::setStart(QPoint pos)
{
    startPos = pos;
}

QPoint Screen::getEnd()
{
    return endPos;
}

QPoint Screen::getStart()
{
    return startPos;
}

QPoint Screen::getLeftUp()
{
    return leftUpPos;
}

QPoint Screen::getRightDown()
{
    return rightDownPos;
}

// 是否在当前选定的截图中
bool Screen::isInArea(QPoint pos)
{
    if (pos.x() > leftUpPos.x() && pos.x() < rightDownPos.x() && pos.y() > leftUpPos.y() && pos.y() < rightDownPos.y()) {
        return true;
    }

    return false;
}

// 截图的移动
void Screen::move(QPoint p)
{
    int lx = leftUpPos.x() + p.x();
    int ly = leftUpPos.y() + p.y();
    int rx = rightDownPos.x() + p.x();
    int ry = rightDownPos.y() + p.y();

    if (lx < 0) {
        lx = 0;
        rx -= p.x();
    }

    if (ly < 0) {
        ly = 0;
        ry -= p.y();
    }

    if (rx > maxWidth)  {
        rx = maxWidth;
        lx -= p.x();
    }

    if (ry > maxHeight) {
        ry = maxHeight;
        ly -= p.y();
    }

    leftUpPos = QPoint(lx, ly);
    rightDownPos = QPoint(rx, ry);
    startPos = leftUpPos;
    endPos = rightDownPos;
}

// 开始位置
void Screen::cmpPoint(QPoint &leftTop, QPoint &rightDown)
{
    QPoint l = leftTop;
    QPoint r = rightDown;

    if (l.x() <= r.x()) {
        if (l.y() <= r.y()) {
            ;
        } else {
            leftTop.setY(r.y());
            rightDown.setY(l.y());
        }
    } else {
        if (l.y() < r.y()) {
            leftTop.setX(r.x());
            rightDown.setX(l.x());
        } else {
            QPoint tmp;
            tmp = leftTop;
            leftTop = rightDown;
            rightDown = tmp;
        }
    }
}

// 定义静态成员self，指向ScreenWidget的智能指针，早对象被销毁的时候自动释放内存
QScopedPointer<ScreenWidget> ScreenWidget::self;
ScreenWidget *ScreenWidget::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new ScreenWidget);
        }
    }

    return self.data();
}

ScreenWidget::ScreenWidget(QWidget *parent) : QWidget(parent)
{
    this->setCursor(Qt::CrossCursor);

    //取得屏幕大小
    screen = new Screen(QApplication::primaryScreen()->size());
    createTab();
    createTab_1();
    //保存全屏图像
    fullScreen = new QPixmap();
}

void ScreenWidget::paintEvent(QPaintEvent *)
{
    // 根据左上角和宽度与高度
    int x = screen->getLeftUp().x();
    int y = screen->getLeftUp().y();
    int w = screen->getRightDown().x() - x;
    int h = screen->getRightDown().y() - y;

    QPainter painter(this);

    // 准备画笔
    QPen pen;
    pen.setColor(Qt::green);
    pen.setWidth(2);
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);

    // 将bgScreen进行呈现
    painter.drawPixmap(0, 0, *bgScreen);

    if (w != 0 && h != 0) {
        QScreen *pscreen = QApplication::primaryScreen();
        qreal scaleFactor = pscreen->devicePixelRatio();
        // 从fullScreen复制截图部分
        painter.drawPixmap(x, y, fullScreen->copy(x * scale, y * scale, w * scale, h * scale));
    }

    painter.drawRect(x, y, w, h);

    pen.setColor(Qt::yellow);
    painter.setPen(pen);
    painter.drawText(x + 2, y - 8, tr("截图范围：( %1 x %2 ) - ( %3 x %4 )  图片大小：( %5 x %6 )")
                     .arg(x).arg(y).arg(x + w).arg(y + h).arg(w).arg(h));
}

void ScreenWidget::showEvent(QShowEvent *)
{
    // 初始化为无效点
    QPoint point(-1, -1);
    screen->setStart(point);
    screen->setEnd(point);

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    *fullScreen = fullScreen->grabWindow(QApplication::desktop()->winId(), 0, 0, screen->width(), screen->height());
#else
    QScreen *pscreen = QApplication::primaryScreen();
    if (pscreen) {
        // 保存下来当前屏幕的截图
        scale = pscreen->devicePixelRatio();
        *fullScreen = pscreen->grabWindow(0, 0, 0, screen->width(), screen->height());
    }
#endif

    // 其实保存了两张图片，一张是原始截图，另一张是模糊了的背景截图
    //设置透明度实现模糊背景
    QPixmap pix(screen->width(), screen->height());
    pix.fill((QColor(160, 160, 160, 120)));
    bgScreen = new QPixmap(*fullScreen);
    QPainter p(bgScreen);
    // 当前界面显示出模糊截图
    p.drawPixmap(0, 0, pix);
}

// 键盘退出
void ScreenWidget::keyPressEvent(QKeyEvent *k)
{
    if (k->key() == Qt::Key_Escape) {
        toolbar_1->hide();
        hide();
    }
}

void ScreenWidget::createTab()
{
    toolbar = new QToolBar(this);
    toolbar->setStyleSheet("QToolBar { background-color: #DCDCDC; border-radius: 10px; }");
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    QToolButton *exit = new QToolButton(this);
    exit->setIcon(QIcon(":/rec/wrong.png"));
    exit->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbar->addWidget(exit);
    toolbar->move(screen->width() / 2 - toolbar->width() / 2, 0);
    connect(exit, &QToolButton::clicked, this, [=](){
        toolbar_1->hide();
        this->hide();
    });
}

void ScreenWidget::createTab_1()
{
    toolbar_1 = new QToolBar(this);
    toolbar_1->setMovable(false);
    toolbar_1->setFloatable(false);
    toolbar_1->setStyleSheet("QToolBar { background-color: #DCDCDC;}");


    // 退出
    QToolButton *exit = new QToolButton(this);
    exit->setIcon(QIcon(":/rec/wrong.png"));
    exit->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbar_1->addWidget(exit);
    exit->setToolTip("退出");
    connect(exit, &QToolButton::clicked, this, [=](){
        toolbar_1->hide();
        this->hide();
    });

    // 完成
    QToolButton *complete = new QToolButton(this);
    complete->setIcon(QIcon(":/rec/yes.png"));
    complete->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbar_1->addWidget(complete);
    complete->setToolTip("完成");
    connect(complete, &QToolButton::clicked, this, [=](){
        // 复制到剪贴板中
        QClipboard *clipboard = QGuiApplication::clipboard();
        QPixmap screenshot;

        // 计算截图的坐标和大小
        int x = screen->getLeftUp().x();
        int y = screen->getLeftUp().y();
        int w = screen->getRightDown().x() - x;
        int h = screen->getRightDown().y() - y;

        // 获取屏幕截图并复制到剪贴板
        screenshot = fullScreen->copy(x * scale, y * scale, w * scale, h * scale);
        clipboard->setPixmap(screenshot);
        toolbar_1->hide();
        this->hide();
    });

    // 另存为
    QToolButton *save = new QToolButton(this);
    save->setIcon(QIcon(":/rec/save.png"));
    save->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbar_1->addWidget(save);
    save->setToolTip("另存为");
    connect(save, &QToolButton::clicked, this, [=](){
        toolbar_1->hide();
        saveScreenOther();
    });

    toolbar_1->hide();

    // ...后期可以添加屏幕识图或者图片翻译、马赛克化等等功能

    connect(this, &ScreenWidget::shotFinish, this, [=](){
        // 是以物理尺寸为基准的
        int bar_height = toolbar_1->height(), bar_width = toolbar_1->width();
        int w = screen->width(), h = screen->height();
        QPoint cur = screen->getEnd();
        QPoint start = screen->getStart();


        if (h - cur.y() < bar_height) {
            QPoint target = {cur.x() - bar_width, start.y()};
            toolbar_1->move(target);
        }
        else {
            QPoint target = {cur.x() - bar_width, cur.y()};
            toolbar_1->move(target);
        }

        toolbar_1->show();
    });
}

// 保存截图
void ScreenWidget::saveScreenOther()
{
    QString name = QString("%1.png").arg(STRDATETIME);
    QString fileName = QFileDialog::getSaveFileName(this, "保存图片", name, "png Files (*.png)");
    if (!fileName.endsWith(".png")) {
        fileName += ".png";
    }

    if (fileName.length() > 0) {
        int x = screen->getLeftUp().x();
        int y = screen->getLeftUp().y();
        int w = screen->getRightDown().x() - x;
        int h = screen->getRightDown().y() - y;
        fullScreen->copy(x * scale, y * scale, w * scale, h * scale).save(fileName, "png");
        hide();
    }
}

void ScreenWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (screen->getStatus() == Screen::SELECT) {
        screen->setEnd(e->pos());
    } else if (screen->getStatus() == Screen::MOV) {
        // 进行截图的移动操作
        toolbar_1->hide();
        QPoint p(e->x() - movPos.x(), e->y() - movPos.y());
        screen->move(p);
        movPos = e->pos();
    }

    this->update();
}

void ScreenWidget::mousePressEvent(QMouseEvent *e)
{
    int status = screen->getStatus();

    if (status == Screen::SELECT) {
        screen->setStart(e->pos());
    } else if (status == Screen::MOV) {
        // 是移动还是重新开始截图
        if (screen->isInArea(e->pos()) == false) {
            screen->setStart(e->pos());
            screen->setStatus(Screen::SELECT);
        } else {
            toolbar_1->hide();
            movPos = e->pos();
            this->setCursor(Qt::SizeAllCursor);
        }
    }

    // 不断地触发重绘操作，也就是paintEvent
    this->update();
}

void ScreenWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (screen->getStatus() == Screen::SELECT) {
        screen->setStatus(Screen::MOV);

    } else if (screen->getStatus() == Screen::MOV) {
        this->setCursor(Qt::CrossCursor);
    }
    emit shotFinish();
}

