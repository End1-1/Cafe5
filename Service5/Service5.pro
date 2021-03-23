QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    configini.cpp \
    database.cpp \
    databaseconnectionmanager.cpp \
    firebase.cpp \
    handlers/authentication.cpp \
    handlers/confirmregistration.cpp \
    handlers/forbidden.cpp \
    handlers/notfound.cpp \
    handlers/registration.cpp \
    handlers/requesthandler.cpp \
    handlers/requestmanager.cpp \
    httpheader.cpp \
    jsonhandler.cpp \
    main.cpp \
    monitor.cpp \
    serverthread.cpp \
    socket/sslserver.cpp \
    socket/sslsocket.cpp \
    socketdata.cpp \
    socketthread.cpp \
    store/storemanager.cpp \
    utils/logwriter.cpp \
    utils/threadworker.cpp

HEADERS += \
    configini.h \
    database.h \
    databaseconnectionmanager.h \
    firebase.h \
    handlers/authentication.h \
    handlers/confirmregistration.h \
    handlers/forbidden.h \
    handlers/notfound.h \
    handlers/registration.h \
    handlers/requesthandler.h \
    handlers/requestmanager.h \
    headers/registrationstate.h \
    httpheader.h \
    jsonhandler.h \
    monitor.h \
    serverthread.h \
    socket/sslserver.h \
    socket/sslsocket.h \
    socketdata.h \
    socketthread.h \
    store/storemanager.h \
    utils/debug.h \
    utils/logwriter.h \
    utils/threadworker.h

FORMS += \
    monitor.ui

INCLUDEPATH += socket
INCLUDEPATH += utils
INCLUDEPATH += handlers
INCLUDEPATH += headers
INCLUDEPATH += store

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
