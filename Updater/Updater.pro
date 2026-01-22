#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T18:48:45
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Updater
TEMPLATE = app

win32 {
    RC_FILE = res.rc
}


ICON = time.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    updatemanger.cpp

HEADERS += \
    updatemanger.h \
    version.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    version_inc.target = version_inc
    version_inc.commands = powershell -NoProfile -ExecutionPolicy Bypass -File $$shell_path($$PWD/increase_version.ps1)
    QMAKE_EXTRA_TARGETS += version_inc
    PRE_TARGETDEPS += version_inc
}


win32 {
    res_rc.depends += $$PWD/version.h
}

DISTFILES += \
    increase_version.ps1
