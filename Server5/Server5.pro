#-------------------------------------------------
#
# Project created by QtCreator 2019-07-19T14:51:08
#
#-------------------------------------------------

QT       += core gui sql network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server5
TEMPLATE = app

RC_FILE = res.rc
ICON = server.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        ../Cafe5/c5database.cpp \
        ../Classes/c5filelogwriter.cpp \
        ../Classes/c5printing.cpp \
        ../Classes/c5printjson.cpp \
        ../Classes/c5threadobject.cpp \
        ../Classes/notificationwidget.cpp \
        c5reportsupload.cpp \
        c5scheduler.cpp \
        dbconnection.cpp \
        main.cpp \
        c5server5.cpp \
        server5settings.cpp \
        serversocket.cpp \
        socketthread.cpp \
        widgetcontainer.cpp

HEADERS += \
        ../Cafe5/c5database.h \
        ../Classes/c5filelogwriter.h \
        ../Classes/c5printing.h \
        ../Classes/c5printjson.h \
        ../Classes/c5threadobject.h \
        ../Classes/notificationwidget.h \
        c5reportsupload.h \
        c5scheduler.h \
        c5server5.h \
        dbconnection.h \
        rc.h \
        res.rc \
        server5settings.h \
        serversocket.h \
        socketthread.h \
        widgetcontainer.h

FORMS += \
        ../Classes/notificationwidget.ui \
        c5reportsupload.ui \
        c5server5.ui \
        dbconnection.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../resources/res.qrc \
    server5.qrc

DEFINES += _ORGANIZATION_=\\\"SmartHotel\\\"
DEFINES += _APPLICATION_=\\\"Server5\\\"
DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _MODULE_=\\\"Server5\\\"

INCLUDEPATH += ../Classes
INCLUDEPATH += ../Cafe5

DISTFILES +=
