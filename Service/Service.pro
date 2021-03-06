QT += core network sql
TARGET = Service
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Classes

SOURCES += \
        ../Cafe5/c5config.cpp \
        ../Cafe5/c5database.cpp \
        ../Cafe5/c5utils.cpp \
        ../Classes/c5storedraftwriter.cpp \
        config.cpp \
        logwriter.cpp \
        main.cpp \
        socketdata.cpp \
        sockethandler.cpp \
        sockethandlergoods.cpp \
        sockethandlerlocationchanged.cpp \
        sockethandlerlogin.cpp \
        sockethandlerserviceconfig.cpp \
        sockethandlerservicelogin.cpp \
        sockethandlerunknown.cpp \
        sockethandleruploadsale.cpp \
        sockethandleruuid.cpp \
        socketrw.cpp \
        sslserver.cpp \
        sslsocket.cpp


LIBS += advapi32.lib

HEADERS += \
    ../Cafe5/c5config.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5utils.h \
    ../Classes/c5storedraftwriter.h \
    config.h \
    logwriter.h \
    servicecommands.h \
    socketdata.h \
    sockethandler.h \
    sockethandlergoods.h \
    sockethandlerlocationchanged.h \
    sockethandlerlogin.h \
    sockethandlerserviceconfig.h \
    sockethandlerservicelogin.h \
    sockethandlerunknown.h \
    sockethandleruploadsale.h \
    sockethandleruuid.h \
    socketrw.h \
    sslserver.h \
    sslsocket.h

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"SERVICE\\\"
DEFINES += _NOAPP_
