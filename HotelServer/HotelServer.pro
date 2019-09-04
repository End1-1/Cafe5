#-------------------------------------------------
#
# Project created by QtCreator 2019-08-19T09:47:10
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HotelServer
TEMPLATE = app

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
        ../HotelFiles/cmd.cpp \
        ../HotelFiles/dateutils.cpp \
        ../HotelFiles/doubleutils.cpp \
        ../HotelFiles/jsonutils.cpp \
        ../HotelFiles/tablewidget.cpp \
        ../HotelFiles/tablewidgetitem.cpp \
        ../HotelFiles/widgetcontainer.cpp \
        ../HotelFiles/widgetcontrols.cpp \
        main.cpp \
        serversocket.cpp \
        serverwindow.cpp \
        socketthread.cpp

HEADERS += \
        ../Cafe5/c5database.h \
        ../HotelFiles/cmd.h \
        ../HotelFiles/dateutils.h \
        ../HotelFiles/doubleutils.h \
        ../HotelFiles/jsonutils.h \
        ../HotelFiles/tablewidget.h \
        ../HotelFiles/tablewidgetitem.h \
        ../HotelFiles/widgetcontainer.h \
        ../HotelFiles/widgetcontrols.h \
        serversocket.h \
        serverwindow.h \
        socketthread.h

FORMS += \
        serverwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../resources/res.qrc

INCLUDEPATH += ..\HotelFiles
INCLUDEPATH += ..\Cafe5

DEFINES += _ORGANIZATION_=\\\"SmartHotel\\\"
DEFINES += _APPLICATION_=\\\"HotelServer\\\"
DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
