#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T15:35:15
#
#-------------------------------------------------

QT += core gui network printsupport sql multimedia widgets websockets concurrent xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include ($$PWD/Frontdesk.pri)

RC_FILE = res.rc

TARGET = OfficeN
TEMPLATE = app

#DEFINES += NEWVERSION

CONFIG += c++11


INCLUDEPATH += c:/Soft/OpenSSLWin64/include
INCLUDEPATH += c:/Soft/OpenSSLWin64/include/openssl

RESOURCES += \
    ../resources/res.qrc \
    lang.qrc

#QMAKE_CXXFLAGS += -Werror

DISTFILES += \
    ../resources/down-arrow.png \
    ../resources/up-arrow.png \
    storehouse.ico

ICON = storehouse.ico

DEFINES += NETWORKDB


LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD


win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

 # win32-g++ {
 #    QMAKE_CXXFLAGS += -Werror
 # }
 # win32-msvc*{
 #    QMAKE_CXXFLAGS += /WX
 # }

FORMS += \
    ../Classes/dlgserverconnection.ui \
    ../Forms/c5goodsspecialprice.ui \
    dlgemarks.ui

HEADERS += \
    ../Classes/dlgserverconnection.h \
    ../Forms/c5goodsspecialprice.h \
    dlgemarks.h

SOURCES += \
    ../Classes/dlgserverconnection.cpp \
    ../Forms/c5goodsspecialprice.cpp \
    dlgemarks.cpp


