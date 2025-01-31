QT -= gui

QT += sql

TEMPLATE = lib
DEFINES += SQLQUERY_LIBRARY
TARGET = jzstore

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sqlquery.cpp

HEADERS += \
    sqlquery.h

SOURCES += \
    ../../handlers/httpheader.cpp \
    ../../utils/commandline.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/logwriter.cpp \


HEADERS += \
    ../../handlers/httpheader.h \
    ../../utils/commandline.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/logwriter.h \


INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils

DEFINES += _APPLICATION_=\\\"ServerNamesElina\\\"
DEFINES += _MODULE_=\\\"Service5_ServerNamesElina_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += REMOTE_ELINA

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
