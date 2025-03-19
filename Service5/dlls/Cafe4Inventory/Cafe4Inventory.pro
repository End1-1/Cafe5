QT -= gui

QT += sql

TEMPLATE = lib
DEFINES += SQLQUERY_LIBRARY
TARGET= cafe4inventory

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../Classes/logwriter.cpp \
    queryjsonresponse.cpp \
    sqlquery.cpp

HEADERS += \
    ../../../Classes/logwriter.h \
    queryjsonresponse.h \
    sqlquery.h

SOURCES += \
    ../../utils/database.cpp \


HEADERS += \
    ../../utils/database.h \


INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils

DEFINES += _APPLICATION_=\\\"SqlQuery\\\"
DEFINES += _MODULE_=\\\"Service5_sqlquery_dll\\\"

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
