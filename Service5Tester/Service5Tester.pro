QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Service5/socket/sslserver.cpp \
    ../Service5/socket/sslsocket.cpp \
    ../Service5/utils/logwriter.cpp \
    ../Service5/utils/threadworker.cpp \
    main.cpp \
    test.cpp \
    testerdialog.cpp \
    testn.cpp \
    testn1.cpp \
    testn2.cpp

HEADERS += \
    ../Service5/socket/sslserver.h \
    ../Service5/socket/sslsocket.h \
    ../Service5/utils/logwriter.h \
    ../Service5/utils/threadworker.h \
    test.h \
    testerdialog.h \
    testn.h \
    testn1.h \
    testn2.h

FORMS += \
    testerdialog.ui

INCLUDEPATH += $$PWD/../Service5/socket
INCLUDEPATH += $$PWD/../Service5/utils
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include/openssl

LIBS += -LC:/Soft/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
