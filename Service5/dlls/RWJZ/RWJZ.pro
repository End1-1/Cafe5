QT -= gui

QT += network sql

TEMPLATE = lib
DEFINES += RWJZ_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../raw/rawmessage.cpp \
    ../../socket/sslsocket.cpp \
    ../../utils/commandline.cpp \
    ../../utils/database.cpp \
    ../../utils/gtranslator.cpp \
    ../../utils/logwriter.cpp \
    ../../utils/tablerecord.cpp \
    rwjz.cpp

HEADERS += \
    ../../raw/rawmessage.h \
    ../../socket/sslsocket.h \
    ../../utils/commandline.h \
    ../../utils/database.h \
    ../../utils/gtranslator.h \
    ../../utils/logwriter.h \
    ../../utils/tablerecord.h \
    rwjz.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _MODULE_=\\\"Service5_rwjz_dll\\\"

INCLUDEPATH += ../../raw
INCLUDEPATH += ../../socket
INCLUDEPATH += ../../utils
INCLUDEPATH += ../../../Classes
