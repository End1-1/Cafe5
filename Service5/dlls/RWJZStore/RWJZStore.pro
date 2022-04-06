QT -= gui

QT += network sql

TEMPLATE = lib
DEFINES += RWJZSTORE_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../raw/rawmessage.cpp \
    ../../socket/sslsocket.cpp \
    rwjzstore.cpp

HEADERS += \
    ../../raw/rawmessage.h \
    ../../socket/sslsocket.h \
    rwjzstore.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += ../../raw
INCLUDEPATH += ../../socket
