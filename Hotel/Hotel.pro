#-------------------------------------------------
#
# Project created by QtCreator 2019-08-15T12:31:51
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hotel
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
        ../HotelFiles/cmd.cpp \
        ../HotelFiles/widgetcontainer.cpp \
        ../HotelFiles/widgetcontrols.cpp \
        chartwindow.cpp \
        clientsocket.cpp \
        connectionsettings.cpp \
        dialog.cpp \
        login.cpp \
        main.cpp \
        dashboard.cpp \
        message.cpp \
        settings.cpp \
        tabwidget.cpp \
        user.cpp

HEADERS += \
        ../HotelFiles/cmd.h \
        ../HotelFiles/widgetcontainer.h \
        ../HotelFiles/widgetcontrols.h \
        chartwindow.h \
        clientsocket.h \
        connectionsettings.h \
        dashboard.h \
        dialog.h \
        login.h \
        message.h \
        settings.h \
        tabwidget.h \
        user.h

FORMS += \
        chartwindow.ui \
        connectionsettings.ui \
        dashboard.ui \
        login.ui \
        message.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

INCLUDEPATH += ..\HotelFiles

DEFINES += _ORGANIZATION_=\\\"SmartHotel\\\"
DEFINES += _APPLICATION_=\\\"Hotel\\\"
DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
