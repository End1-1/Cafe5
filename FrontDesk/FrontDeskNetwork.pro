#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T15:35:15
#
#-------------------------------------------------

QT += core gui network printsupport sql multimedia widgets websockets concurrent xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include ($$PWD/Frontdesk.pri)

win32 {
    RC_FILE = res.rc
}


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
    increase_version.ps1 \
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


win32 {
    version_inc.target = version_inc
    version_inc.commands = powershell -NoProfile -ExecutionPolicy Bypass -File $$shell_path($$PWD/increase_version.ps1)
    QMAKE_EXTRA_TARGETS += version_inc
    PRE_TARGETDEPS += version_inc
}


win32 {
    res_rc.depends += $$PWD/version.h
}

