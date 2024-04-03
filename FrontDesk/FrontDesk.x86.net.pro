#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T15:35:15
#
#-------------------------------------------------

QT       += core gui network printsupport sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_FILE = res.rc

TARGET = Office
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"FRONTDESK\\\"
DEFINES += FRONTDESK
DEFINES += NETWORKDB

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include ($$PWD/Frontdesk.pri)



INCLUDEPATH += /Soft/OpenSSL-Win32/include
INCLUDEPATH += /Soft/OpenSSL-Win32/include/openssl

RESOURCES += \
    ../resources/res.qrc \
    lang.qrc

#QMAKE_CXXFLAGS += -Werror

DISTFILES += \
    ../resources/down-arrow.png \
    ../resources/up-arrow.png \
    storehouse.ico

ICON = storehouse.ico

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/Soft/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings
