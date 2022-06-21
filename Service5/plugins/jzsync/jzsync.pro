QT += sql network

QT -= gui

TEMPLATE = lib
DEFINES += JZSYNC_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/databaseconnectionmanager.cpp \
    ../../utils/jsonhandler.cpp \
    ../../utils/logwriter.cpp \
    jzsync.cpp \
    jzsyncmanager.cpp

HEADERS += \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/databaseconnectionmanager.h \
    ../../utils/jsonhandler.h \
    ../../utils/logwriter.h \
    jzsync.h \
    jzsyncmanager.h

DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _MODULE_=\\\"Service5_jzsync_dll\\\"

INCLUDEPATH += ../../raw
INCLUDEPATH += ../../socket
INCLUDEPATH += ../../utils
INCLUDEPATH += ../../../Classes

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
