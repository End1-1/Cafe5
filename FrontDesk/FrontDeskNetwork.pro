#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T15:35:15
#
#-------------------------------------------------

QT += core gui network printsupport sql multimedia widgets websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include ($$PWD/Frontdesk.pri)

RC_FILE = res.rc

TARGET = OfficeN
TEMPLATE = app
#
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.


#DEFINES += NEWVERSION

CONFIG += c++11

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


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

HEADERS += \
    ../Classes/amkbd.h \
    ../Classes/logwriter.h \
    ../Forms/c5goodsprice.h \
    ../NTable/ninterface.h

SOURCES += \
    ../Classes/amkbd.cpp \
    ../Classes/logwriter.cpp \
    ../Forms/c5goodsprice.cpp \
    ../NTable/ninterface.cpp

FORMS += \
    ../Forms/c5goodsprice.ui
