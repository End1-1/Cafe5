QT       += core gui widgets quick quickwidgets positioning location

CONFIG += c++11
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Service5/raw/rawmessage.cpp \
    ../Service5/socket/sslsocket.cpp \
    connections.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    socketconnection.cpp \
    socketwidget.cpp

HEADERS += \
    ../Service5/raw/messagelist.h \
    ../Service5/raw/rawmessage.h \
    ../Service5/raw/structs.h \
    ../Service5/socket/sslsocket.h \
    connections.h \
    mainwindow.h \
    map.h \
    socketconnection.h \
    socketwidget.h

FORMS += \
    connections.ui \
    mainwindow.ui \
    map.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    qmap.qml

RESOURCES += \
    ../resources/res_service5monitor.qrc \
    res.qrc

INCLUDEPATH += ../Service5/raw
INCLUDEPATH += ../Service5/socket
