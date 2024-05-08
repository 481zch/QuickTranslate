#include "queryinterface.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

QueryInterface::QueryInterface(QPoint pos, MyDataBase* db, QWidget *parent)
    : QMainWindow(parent), db(db), pos(pos),
    ttsToAppLanguageMap({
          {"zh", "zh"}, {"en", "en"}, {"jp", "ja"}, {"kor", "ko"}, {"th", "th"}, {"vie", "vi"}, {"ru", "ru"}
      })
{
    setupUi();
    setupConnections();
}

QueryInterface::~QueryInterface() {
    delete tts;
}

void QueryInterface::setupUi() {
    // 布局初始化
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    move(pos);

    play = new MyPushButton(this);
    input = new MyLineEdit(this);
    output = new MyLineEdit(this);

    input->setGeometry(10, 10, 180, 30);
    output->setGeometry(10, 50, 180, 30);

    int a = this->width(), b = this->height();
    play->setGeometry(140, 90, 40, 18);
    output->setReadOnly(true);

    input->setPlaceholderText("请输入内容(回车确认)");
    output->setPlaceholderText("输出为：");
    play->setText("发音");

    curRes = db->getResData();

    setLanguage = {curRes[2], curRes[3]};
    getLanguage = new GetLanguage(curRes[4], curRes[5], this);

    getLanguage->startWarmUp();

    // 最耗时的步骤
    network = new NetWork(curRes[4], curRes[5]);
    network->startWarmUp();

    tts = new QTextToSpeech(this);
}

void QueryInterface::setupConnections() {
    connect(play, &MyPushButton::clicked, this, [=]() {
        if (!output->text().isEmpty()) {
            // 需要一个映射表
            QString ttsLanguage = ttsToAppLanguageMap[toLanguage];
            QLocale locale(ttsLanguage);
            tts->setLocale(locale);
            tts->say(output->text());
        }
    });

    connect(getLanguage, &GetLanguage::returnLanguage, this, &QueryInterface::handleLanguageDetection);
    connect(network, &NetWork::translateFinish, this, &QueryInterface::handleTranslationFinished);
}

void QueryInterface::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.setClipPath(path);

    if (scaledPix.isNull()) {
        QPixmap pix(curRes[1]);
        pix = pix.scaled(this->width(), this->height(), Qt::KeepAspectRatioByExpanding);
        scaledPix = pix;
        painter.drawPixmap(0, 0, pix);
        setFixedSize(pix.width(), pix.height());
    }
    else
    {
        painter.drawPixmap(0, 0, scaledPix);
        setFixedSize(scaledPix.width(), scaledPix.height());
    }
}


void QueryInterface::changeEvent(QEvent *event) {
    if (event->type() == QEvent::ActivationChange && !isActiveWindow()) {
        input->clear();
        output->clear();
        emit changeFocus();
    }
    QMainWindow::changeEvent(event);
}

void QueryInterface::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QString curText = input->text();
        qDebug() << "目前语言为：" << curText;
        getLanguage->achieveLanguage(curText);
    }
    QMainWindow::keyPressEvent(event);
}

void QueryInterface::handleLanguageDetection(QString src) {
    fromLanguage = src;
    emit getLanguageTypeFinish();
    if (fromLanguage != setLanguage.first && fromLanguage != setLanguage.second) {
        output->setText("请正确设置语言");
    }
    else {
        toLanguage = (fromLanguage == setLanguage.first) ? setLanguage.second : setLanguage.first;
        network->setParameters(input->text(), fromLanguage, toLanguage);
        network->translateText();
    }
}

void QueryInterface::handleTranslationFinished(QString result) {
    qDebug() << "翻译结果为：" << result;
    output->setText(result);
    db->insertTranslateData(fromLanguage, toLanguage, input->text(), result, QDateTime::currentDateTime());
}

void QueryInterface::changePaper(QString path)
{
    curRes[1] = path;
    scaledPix = QPixmap();
    this->update();
}

void QueryInterface::changeLanguage(QString from, QString to)
{
    setLanguage = {from, to};
}

void QueryInterface::setLocation(QPoint cur)
{
    move(cur);
}
