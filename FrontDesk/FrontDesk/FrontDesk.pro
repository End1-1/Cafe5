#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T15:35:15
#
#-------------------------------------------------

QT       += core gui network printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FrontDesk
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"FRONTDESK\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        c5mainwindow.cpp \
    ../../Cafe5/c5config.cpp \
    ../../Cafe5/c5connection.cpp \
    ../../Cafe5/c5database.cpp \
    ../../Cafe5/c5dialog.cpp \
    ../../Cafe5/c5message.cpp \
    ../../Cafe5/c5sqlquery.cpp \
    ../../Cafe5/c5staticdb.cpp \
    ../../Cafe5/c5utils.cpp

HEADERS += \
        c5mainwindow.h \
    ../../Cafe5/c5config.h \
    ../../Cafe5/c5connection.h \
    ../../Cafe5/c5database.h \
    ../../Cafe5/c5dialog.h \
    ../../Cafe5/c5message.h \
    ../../Cafe5/c5sqlquery.h \
    ../../Cafe5/c5staticdb.h \
    ../../Cafe5/c5utils.h

FORMS += \
        c5mainwindow.ui \
    ../../Cafe5/c5connection.ui \
    ../../Cafe5/c5message.ui

INCLUDEPATH += ../../Cafe5
