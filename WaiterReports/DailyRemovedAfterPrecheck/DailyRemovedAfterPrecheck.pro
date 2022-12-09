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
INCLUDEPATH += ../../Controls

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../Classes/notificationwidget.cpp \
    dll.cpp \
    ../../Cafe5/c5database.cpp \
    ../../Cafe5/c5config.cpp \
    ../../Cafe5/c5utils.cpp \
    ../../Classes/c5printing.cpp \
    ../../Controls/c5checkbox.cpp \
    ../../Controls/c5lineedit.cpp

HEADERS += \
    ../../Classes/notificationwidget.h \
    dll.h \
    ../../Cafe5/c5database.h \
    ../../Cafe5/c5config.h \
    ../../Cafe5/c5utils.h \
    ../../Classes/c5printing.h \
    ../../Controls/c5checkbox.h \
    ../../Controls/c5lineedit.h

FORMS += \
    ../../Classes/notificationwidget.ui

RESOURCES += \
    res.qrc
