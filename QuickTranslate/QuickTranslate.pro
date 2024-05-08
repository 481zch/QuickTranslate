QT       += core gui
QT       += texttospeech
QT       += sql
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    getlanguage.cpp \
    main.cpp \
    floatingball.cpp \
    mydatabase.cpp \
    mylineedit.cpp \
    mypushbutton.cpp \
    network.cpp \
    queryinterface.cpp \
    screenwidget.cpp \
    socketserver.cpp

HEADERS += \
    floatingball.h \
    getlanguage.h \
    mydatabase.h \
    mylineedit.h \
    mypushbutton.h \
    network.h \
    queryinterface.h \
    screenwidget.h \
    socketserver.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

win32: RC_ICONS += "cat1.ico"

DISTFILES +=


