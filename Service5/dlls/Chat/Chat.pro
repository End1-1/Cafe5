QT -= gui

QT += sql network printsupport

TEMPLATE = lib
DEFINES += SQLQUERY_LIBRARY
TARGET = chat

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sqlquery.cpp \
    ../../handlers/httpheader.cpp \
    ../../utils/commandline.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/logwriter.cpp


HEADERS += \
    sqlquery.h \
    ../../handlers/httpheader.h \
    ../../utils/commandline.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/logwriter.h


INCLUDEPATH += C:/Projects/Cafe5/Cafe5
INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils
INCLUDEPATH += c:/projects/cafe5/service5working/utils

LIBS += -lVersion

DEFINES += _APPLICATION_=\\\"Picasso\\\"
DEFINES += _MODULE_=\\\"Service5_chat_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
