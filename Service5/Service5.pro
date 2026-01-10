QT       += core gui network sql websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20
TARGET = service5
RC_FILE = res.rc

SOURCES += \
    ../Classes/fileversion.cpp \
    ../Classes/logwriter.cpp \
    armsoft.cpp \
    c5searchengine.cpp \
    utils/configini.cpp \
    utils/database.cpp \
    datadriver/datadriver.cpp \
    dlglicenses.cpp \
    firebase.cpp \
    handlers/httpheader.cpp \
    serverthread.cpp \
    utils/sqlqueries.cpp \
    waiter.cpp \
    worker/socketstruct.cpp

HEADERS += \
    ../Classes/fileversion.h \
    ../Classes/logwriter.h \
    armsoft.h \
    c5searchengine.h \
    struct_storage_item.h \
    utils/configini.h \
    utils/database.h \
    datadriver/datadriver.h \
    dlglicenses.h \
    firebase.h \
    handlers/httpheader.h \
    rc.h \
    serverthread.h \
    utils/sqlqueries.h \
    waiter.h \
    worker/socketstruct.h \
    worker/struct_goods_item.h \
    worker/struct_hall.h \
    worker/struct_partner.h \
    worker/struct_storage_item.h

FORMS += \
    dlglicenses.ui

win32 {
    SOURCES += main.cpp
}
unix {
    SOURCES += main_linux.cpp
}

INCLUDEPATH += handlers
INCLUDEPATH += headers
INCLUDEPATH += store
INCLUDEPATH += datadriver
INCLUDEPATH += utils
INCLUDEPATH += worker
INCLUDEPATH += ../Service5Working/raw
INCLUDEPATH += ../Service5Working/socket
INCLUDEPATH += ../Classes

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"BreezeDevs\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"Service5\\\"
DEFINES += QSSLSOCKET_DEBUG

win32 {

        LIBS += -lVersion
        LIBS += -lwsock32
        LIBS += advapi32.lib
}

win32 {
QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:"level='requireAdministrator'"
}

linux {
    VERSION = 1.4.5.53
    QMAKE_LFLAGS += -Wl,--build-id
    DEFINES += APP_VERSION=\\\"$$VERSION\\\"
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

