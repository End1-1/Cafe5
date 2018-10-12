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
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5staticdb.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5sockethandler.cpp \
    c5login.cpp \
    ../Classes/c5permissions.cpp \
    c5reportwidget.cpp \
    ../Classes/c5grid.cpp \
    ../Reports/cr5usersgroups.cpp \
    ../Reports/cr5users.cpp \
    ../Classes/c5tablemodel.cpp

HEADERS += \
        c5mainwindow.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5staticdb.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5sockethandler.h \
    c5login.h \
    ../Classes/c5permissions.h \
    c5reportwidget.h \
    ../Classes/c5grid.h \
    ../Reports/cr5usersgroups.h \
    ../Reports/cr5users.h \
    ../Classes/c5tablemodel.h

FORMS += \
        c5mainwindow.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    c5login.ui \
    ../Classes/c5grid.ui

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Reports
INCLUDEPATH += ../RESOURCES

RESOURCES += \
    ../resources/res.qrc

QMAKE_CXXFLAGS += -Werror
