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
    ../../../Classes/logwriter.cpp \
    sqlquery.cpp \
    ../../handlers/httpheader.cpp \
    ../../utils/commandline.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \


HEADERS += \
    ../../../Classes/logwriter.h \
    sqlquery.h \
    ../../handlers/httpheader.h \
    ../../utils/commandline.h \
    ../../utils/configini.h \
    ../../utils/database.h \


INCLUDEPATH += ../../../Cafe5
INCLUDEPATH += ../../../Cafe5/Classes
INCLUDEPATH += ../../../Service5/handlers
INCLUDEPATH += ../../../Service5/utils
INCLUDEPATH += ../../../service5working/utils

LIBS += -lVersion

DEFINES += _APPLICATION_=\\\"Picasso\\\"
DEFINES += _MODULE_=\\\"Service5_chat_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
