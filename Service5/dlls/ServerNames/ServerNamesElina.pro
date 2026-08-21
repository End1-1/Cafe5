QT -= gui

TEMPLATE = lib
CONFIG += c++20 plugin
TARGET = ServerNames

DEFINES += SQLQUERY_LIBRARY
DEFINES += REMOTE_ELINA
DEFINES += _APPLICATION_=\\\"ServerNamesElina\\\"
DEFINES += _MODULE_=\\\"Service5_ServerNamesElina_dll\\\"

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
