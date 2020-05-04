#-------------------------------------------------
#
# Project created by QtCreator 2020-05-03T16:51:54
#
#-------------------------------------------------

QT       += sql printsupport network

QT       -= gui

greaterThan(QT_MAJOR_VERSION, 4):

TARGET = shop_printreceipt
TEMPLATE = lib

DEFINES += SHOP_PRINTRECEIPT_LIBRARY

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"PRINTRECEIPT\\\"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../Cafe5/c5database.cpp \
        ../../../Classes/c5printing.cpp \
        shop_printreceipt.cpp

HEADERS += \
        ../../../Cafe5/c5database.h \
        ../../../Classes/c5printing.h \
        shop_printreceipt.h \
        shop_printreceipt_global.h 

INCLUDEPATH += C:/Projects/Cafe5/Cafe5
INCLUDEPATH += C:/Projects/Cafe5/Classes

unix {
    target.path = /usr/lib
    INSTALLS += target
}
