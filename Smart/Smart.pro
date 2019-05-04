#-------------------------------------------------
#
# Project created by QtCreator 2018-10-26T15:29:15
#
#-------------------------------------------------

QT       += core gui network printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Smart
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += WAITER

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../../NewTax/printtaxn.cpp \
    dishitemdelegate.cpp \
        main.cpp \
    payment.cpp \
        workspace.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5sockethandler.cpp \
    ../Waiter/dlgpassword.cpp \
    ../Controls/c5lineedit.cpp \
    ../Classes/c5user.cpp

HEADERS += \
    ../../NewTax/printtaxn.h \
    dish.h \
    dishitemdelegate.h \
    payment.h \
        workspace.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5sockethandler.h \
    ../Cafe5/c5socketmessage.h \
    ../Waiter/dlgpassword.h \
    ../Controls/c5lineedit.h \
    ../Classes/c5user.h

FORMS += \
    ../FrontDesk/c5selector.ui \
    payment.ui \
        workspace.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    ../Waiter/dlgpassword.ui

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"SMART\\\"
DEFINES += WAITER

INCLUDEPATH += c:/projects/cafe5/Cafe5
INCLUDEPATH += c:/projects/cafe5/Classes
INCLUDEPATH += c:/projects/cafe5/Controls
INCLUDEPATH += c:/projects/cafe5/Waiter
INCLUDEPATH += c:/Projects/NewTax
INCLUDEPATH += C:/OpenSSL-Win32/include
INCLUDEPATH += C:/OpenSSL-Win32/include/openssl

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto

RESOURCES += \
    ../resources/resources.qrc
