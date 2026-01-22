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

FORMS += \
    ../Forms/c5connectiondialog.ui \
    ../Forms/c5fiscalcancel.ui \
    ../Forms/c5goodsspecialprice.ui \
    ../Forms/c5menu.ui \
    ../Forms/dlgimportfromas.ui \
    c5discountredeem.ui \
    dlgemarks.ui \
    dwaiterworkstationconfig.ui \
    rabstracteditordialog.ui \
    rabstracteditorreport.ui \
    rwaiterstationconfig.ui \
    storedecomplectation.ui \
    wdashboard.ui

HEADERS += \
    ../Classes/appwebsocket.h \
    ../Classes/c5htmlprint.h \
    ../Classes/dict_dish_state.h \
    ../Forms/c5connectiondialog.h \
    ../Forms/c5fiscalcancel.h \
    ../Forms/c5goodsspecialprice.h \
    ../Forms/c5menu.h \
    ../Forms/dlgimportfromas.h \
    ../StructModel/c5structmodel.h \
    c5discountredeem.h \
    c5officedialog.h \
    c5officewidget.h \
    dlgemarks.h \
    dwaiterworkstationconfig.h \
    rabstracteditordialog.h \
    rabstracteditorreport.h \
    rabstractspecialwidget.h \
    rwaiterstationconfig.h \
    storedecomplectation.h \
    version.h \
    wdashboard.h

SOURCES += \
    ../Classes/appwebsocket.cpp \
    ../Classes/c5htmlprint.cpp \
    ../Forms/c5connectiondialog.cpp \
    ../Forms/c5fiscalcancel.cpp \
    ../Forms/c5goodsspecialprice.cpp \
    ../Forms/c5menu.cpp \
    ../Forms/dlgimportfromas.cpp \
    c5discountredeem.cpp \
    c5officedialog.cpp \
    c5officewidget.cpp \
    dlgemarks.cpp \
    dwaiterworkstationconfig.cpp \
    rabstracteditordialog.cpp \
    rabstracteditorreport.cpp \
    rabstractspecialwidget.cpp \
    rwaiterstationconfig.cpp \
    storedecomplectation.cpp \
    wdashboard.cpp

win32 {
    version_inc.target = version_inc
    version_inc.commands = powershell -NoProfile -ExecutionPolicy Bypass -File $$shell_path($$PWD/increase_version.ps1)
    QMAKE_EXTRA_TARGETS += version_inc
    PRE_TARGETDEPS += version_inc
}


win32 {
    res_rc.depends += $$PWD/version.h
}
