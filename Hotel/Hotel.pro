#-------------------------------------------------
#
# Project created by QtCreator 2019-08-15T12:31:51
#
#-------------------------------------------------

QT       += core gui network sql printsupport

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
        ../Cafe5/c5config.cpp \
        ../Cafe5/c5database.cpp \
        ../Cafe5/c5utils.cpp \
        ../Controls/c5dateedit.cpp \
        ../Controls/c5lineedit.cpp \
        ../HotelFiles/cmd.cpp \
        ../HotelFiles/widgetcontainer.cpp \
        ../HotelFiles/widgetcontrols.cpp \
        chartdatescene.cpp \
        chartitemmonthrect.cpp \
        chartitemrect.cpp \
        chartparams.cpp \
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
        ../Cafe5/c5config.h \
        ../Cafe5/c5database.h \
        ../Cafe5/c5utils.h \
        ../Controls/c5dateedit.h \
        ../Controls/c5lineedit.h \
        ../HotelFiles/cmd.h \
        ../HotelFiles/widgetcontainer.h \
        ../HotelFiles/widgetcontrols.h \
        chartdatescene.h \
        chartitemmonthrect.h \
        chartitemrect.h \
        chartparams.h \
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
INCLUDEPATH += ..\Controls
INCLUDEPATH += ..\Cafe5

DEFINES += _ORGANIZATION_=\\\"SmartHotel\\\"
DEFINES += _APPLICATION_=\\\"Hotel\\\"
DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _MODULE_=\\\"HOTEL\\\"
