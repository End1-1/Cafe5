QT += sql gui core widgets network printsupport

CONFIG += dll

TEMPLATE = lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

RC_FILE = res.rc

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"WAITER\\\"
DEFINES += DLL

INCLUDEPATH += ../../Cafe5
INCLUDEPATH += ../../Classes

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dll.cpp \
    ../../Cafe5/c5database.cpp \
    ../../Cafe5/c5message.cpp \
    ../../Cafe5/c5config.cpp \
    ../../Cafe5/c5utils.cpp \
    ../../Classes/c5printing.cpp

HEADERS += \
    dll.h \
    ../../Cafe5/c5database.h \
    ../../Cafe5/c5message.h \
    ../../Cafe5/c5config.h \
    ../../Cafe5/c5utils.h \
    ../../Classes/c5printing.h

FORMS += \
    ../../Cafe5/c5message.ui

RESOURCES += \
    res.qrc
