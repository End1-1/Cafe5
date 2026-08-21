QT -= gui

TEMPLATE = lib
CONFIG += c++20 plugin
TARGET = ServerNames

DEFINES += SQLQUERY_LIBRARY
DEFINES += REMOTE_ALL
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"Service5_ServerNames_dll\\\"

SOURCES += \
    sqlquery.cpp \
    ../../../Classes/logwriter.cpp

HEADERS += \
    sqlquery.h \
    ../../../Classes/logwriter.h

INCLUDEPATH += $$PWD/../../../Classes

unix {
    target.path = /opt/service5
}
!isEmpty(target.path): INSTALLS += target
