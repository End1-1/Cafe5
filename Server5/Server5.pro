#-------------------------------------------------
#
# Project created by QtCreator 2019-07-19T14:51:08
#
#-------------------------------------------------

QT       += core gui sql network printsupport websockets multimedia

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
        ../Cafe5/c5config.cpp \
        ../Cafe5/c5database.cpp \
        ../Cafe5/c5message.cpp \
        ../Cafe5/c5utils.cpp \
        ../Classes/QProgressIndicator.cpp \
        ../Classes/appwebsocket.cpp \
        ../Classes/c5networkdb.cpp \
        ../Classes/c5printing.cpp \
        ../Classes/c5printjson.cpp \
        ../Classes/c5threadobject.cpp \
        ../Classes/notificationwidget.cpp \
        ../NTable/ndataprovider.cpp \
        ../NTable/ninterface.cpp \
        ../NTable/nloadingdlg.cpp \
        ../Service5Working/utils/logwriter.cpp \
        c5scheduler.cpp \
        c5serverprinter.cpp \
        main.cpp \
        c5server5.cpp \
        server5settings.cpp \
        widgetcontainer.cpp

HEADERS += \
        ../Cafe5/C5Database.h \
        ../Cafe5/c5config.h \
        ../Cafe5/c5message.h \
        ../Cafe5/c5utils.h \
        ../Classes/QProgressIndicator.h \
        ../Classes/appwebsocket.h \
        ../Classes/c5networkdb.h \
        ../Classes/c5printing.h \
        ../Classes/c5printjson.h \
        ../Classes/c5threadobject.h \
        ../Classes/notificationwidget.h \
        ../NTable/ndataprovider.h \
        ../NTable/ninterface.h \
        ../NTable/nloadingdlg.h \
        ../Service5Working/utils/logwriter.h \
        c5scheduler.h \
        c5server5.h \
        c5serverprinter.h \
        rc.h \
        res.rc \
        server5settings.h \
        widgetcontainer.h

FORMS += \
        ../Cafe5/c5message.ui \
        ../Classes/notificationwidget.ui \
        ../NTable/nloadingdlg.ui \
        c5server5.ui

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
INCLUDEPATH += ../NTable
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils

