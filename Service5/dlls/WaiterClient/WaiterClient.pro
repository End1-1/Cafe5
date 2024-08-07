QT -= gui

QT += network sql

TEMPLATE = lib
DEFINES += WAITERCLIENT_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../raw/rawmessage.cpp \
    ../../socket/sslsocket.cpp \
    ../../utils/commandline.cpp \
    ../../utils/database.cpp \
    ../../utils/logwriter.cpp \
    ../../utils/networktable.cpp \
    dishes.cpp \
    halls.cpp \
    login.cpp \
    ops.cpp \
    order.cpp \
    translator.cpp \
    waiterclient.cpp

HEADERS += \
    ../../raw/rawmessage.h \
    ../../socket/sslsocket.h \
    ../../utils/commandline.h \
    ../../utils/database.h \
    ../../utils/logwriter.h \
    ../../utils/networktable.h \
    dishes.h \
    halls.h \
    login.h \
    ops.h \
    order.h \
    translator.h \
    waiterclient.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _MODULE_=\\\"Service5_waiterclient_dll\\\"

INCLUDEPATH += ../../raw
INCLUDEPATH += ../../socket
INCLUDEPATH += ../../utils
INCLUDEPATH += ../../../Classes
