QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    monitor.cpp \
    serverthread.cpp \
    socket/sslserver.cpp \
    socket/sslsocket.cpp \
    socketthread.cpp \
    utils/logwriter.cpp \
    utils/threadworker.cpp

HEADERS += \
    monitor.h \
    serverthread.h \
    socket/sslserver.h \
    socket/sslsocket.h \
    socketthread.h \
    utils/logwriter.h \
    utils/threadworker.h

FORMS += \
    monitor.ui

INCLUDEPATH += socket
INCLUDEPATH += utils

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
