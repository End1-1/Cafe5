QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Service5/raw/rawmessage.cpp \
    ../Service5/socket/sslserver.cpp \
    ../Service5/socket/sslsocket.cpp \
    ../Service5/utils/configini.cpp \
    ../Service5/utils/logwriter.cpp \
    main.cpp \
    socketconnection.cpp \
    test.cpp \
    testerdialog.cpp

HEADERS += \
    ../Service5/raw/messagelist.h \
    ../Service5/raw/rawmessage.h \
    ../Service5/raw/structs.h \
    ../Service5/socket/sslserver.h \
    ../Service5/socket/sslsocket.h \
    ../Service5/utils/configini.h \
    ../Service5/utils/debug.h \
    ../Service5/utils/logwriter.h \
    socketconnection.h \
    test.h \
    testerdialog.h

FORMS += \
    testerdialog.ui


INCLUDEPATH += ../Service5/raw
INCLUDEPATH += ../Service5/socket
INCLUDEPATH += $$PWD/../Service5/utils
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include/openssl

LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += -llibcrypto

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"Service5Tester\\\"
