QT += core network sql
TARGET = Service
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += ../Cafe5

SOURCES += \
        ../Cafe5/c5database.cpp \
        config.cpp \
        logwriter.cpp \
        main.cpp \
        sockethandler.cpp \
        sockethandlerlogin.cpp \
        sockethandlerloginwithsession.cpp \
        sockethandlerserviceconfig.cpp \
        sockethandlerservicelogin.cpp \
        sockethandlerunknown.cpp \
        socketrw.cpp \
        sslserver.cpp \
        sslsocket.cpp


LIBS += advapi32.lib

HEADERS += \
    ../Cafe5/c5database.h \
    config.h \
    logwriter.h \
    servicecommands.h \
    sockethandler.h \
    sockethandlerlogin.h \
    sockethandlerloginwithsession.h \
    sockethandlerserviceconfig.h \
    sockethandlerservicelogin.h \
    sockethandlerunknown.h \
    socketrw.h \
    sslserver.h \
    sslsocket.h

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"SERVICE\\\"
DEFINES += _NOAPP_
