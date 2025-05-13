#-------------------------------------------------
#
# Project created by QtCreator 2018-10-26T15:29:15
#
#-------------------------------------------------

QT       += core gui network printsupport sql multimedia websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Smart
TEMPLATE = app

RC_FILE = res.rc

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += WAITER
DEFINES += NETWORKDB
DEFINES += SMART

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


include ($$PWD/Smart.pri)

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"SMART\\\"
DEFINES += WAITER

INCLUDEPATH += c:/projects/cafe5/Cafe5
INCLUDEPATH += c:/projects/cafe5/Classes
INCLUDEPATH += c:/projects/cafe5/Controls
INCLUDEPATH += c:/projects/cafe5/Service
INCLUDEPATH += c:/projects/cafe5/TableRecord
INCLUDEPATH += c:/projects/cafe5/service5working/utils
INCLUDEPATH += c:/projects/cafe5/DbData
INCLUDEPATH += c:/projects/cafe5/Forms
INCLUDEPATH += c:/Projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSLWin64/include
INCLUDEPATH += C:/Soft/OpenSSLWin64/include/openssl

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += -llibcrypto

RESOURCES += \
    ../resources/resources.qrc \
    transres.qrc

FORMS += \
    ../Classes/dlgserverconnection.ui \
    ../NTable/nloadingdlg.ui \
    dlgservicevalues.ui

HEADERS += \
    ../Classes/QProgressIndicator.h \
    ../Classes/amkbd.h \
    ../Classes/c5logtoserverthread.h \
    ../Classes/dlgserverconnection.h \
    ../NTable/ndataprovider.h \
    ../NTable/ninterface.h \
    ../NTable/nloadingdlg.h \
    dlgservicevalues.h

SOURCES += \
    ../../NewTax/Src/printtaxn.cpp \
    ../Classes/QProgressIndicator.cpp \
    ../Classes/amkbd.cpp \
    ../Classes/dlgserverconnection.cpp \
    ../NTable/ndataprovider.cpp \
    ../NTable/ninterface.cpp \
    ../NTable/nloadingdlg.cpp \
    ../Service5Working/utils/logwriter.cpp \
    dlgservicevalues.cpp

win32-g++ {
   QMAKE_CXXFLAGS += -Werror
}
win32-msvc*{
   QMAKE_CXXFLAGS += /WX
}
