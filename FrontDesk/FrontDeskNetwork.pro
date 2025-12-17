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
    ../Forms/c5fiscalcancel.ui \
    ../Forms/c5goodsspecialprice.ui \
    ../Forms/dlgimportfromas.ui \
    c5discountredeem.ui \
    dlgemarks.ui \
    storedecomplectation.ui

HEADERS += \
    ../Classes/dlgserverconnection.h \
    ../Forms/c5fiscalcancel.h \
    ../Forms/c5goodsspecialprice.h \
    ../Forms/dlgimportfromas.h \
    c5discountredeem.h \
    c5officedialog.h \
    c5officewidget.h \
    dlgemarks.h \
    storedecomplectation.h

SOURCES += \
    ../Classes/dlgserverconnection.cpp \
    ../Forms/c5fiscalcancel.cpp \
    ../Forms/c5goodsspecialprice.cpp \
    ../Forms/dlgimportfromas.cpp \
    c5discountredeem.cpp \
    c5officedialog.cpp \
    c5officewidget.cpp \
    dlgemarks.cpp \
    storedecomplectation.cpp


