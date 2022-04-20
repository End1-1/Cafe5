QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Classes/c5crypt.cpp \
    ../Classes/config.cpp \
    ../Classes/socketconnection.cpp \
    ../Forms/dlgserversettings.cpp \
    ../Service5/raw/rawmessage.cpp \
    ../Service5/socket/sslsocket.cpp \
    main.cpp \
    mainwindow.cpp \
    socketwidget.cpp \
    tablemodel.cpp \
    tablewidget.cpp

HEADERS += \
    ../Classes/c5crypt.h \
    ../Classes/config.h \
    ../Classes/socketconnection.h \
    ../Forms/dlgserversettings.h \
    ../Service5/raw/messagelist.h \
    ../Service5/raw/rawmessage.h \
    ../Service5/socket/sslsocket.h \
    mainwindow.h \
    socketwidget.h \
    tablemodel.h \
    tablewidget.h

FORMS += \
    ../Forms/dlgserversettings.ui \
    mainwindow.ui \
    tablewidget.ui

INCLUDEPATH += ../Classes
INCLUDEPATH += ../Forms
INCLUDEPATH += ../Service5/raw
INCLUDEPATH += ../Service5/socket
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include/openssl

LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -llibcrypto

DEFINES += _ORGANIZATION_=\\\"BreezeDevs\\\"
DEFINES += _APPLICATION_=\\\"Office\\\"
DEFINES += _CRYPT_KEY_=\\\"office2022!!!\\\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../resources/office_res.qrc
