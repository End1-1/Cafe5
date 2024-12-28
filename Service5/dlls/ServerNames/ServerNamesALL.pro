QT -= gui

QT += sql

TEMPLATE = lib
DEFINES += SQLQUERY_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../Classes/logwriter.cpp \
    sqlquery.cpp

HEADERS += \
    ../../../Classes/logwriter.h \
    sqlquery.h

SOURCES += \
    ../../utils/commandline.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \


HEADERS += \
    ../../utils/commandline.h \
    ../../utils/configini.h \
    ../../utils/database.h \


INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils

DEFINES += _APPLICATION_=\\\"ElinaSKUQty\\\"
DEFINES += _MODULE_=\\\"Service5_ServerNames_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += REMOTE_ALL

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
