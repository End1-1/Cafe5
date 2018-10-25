#-------------------------------------------------
#
# Project created by QtCreator 2018-06-21T10:59:55
#
#-------------------------------------------------

QT       += core gui network printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Waiter
TEMPLATE = app

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"WAITER\\\"

QMAKE_CXXFLAGS += -Werror

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
        main.cpp \
        dlgface.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5sockethandler.cpp \
    ../Cafe5/c5serverhandler.cpp \
    dlgpassword.cpp \
    dlgorder.cpp \
    ../Classes/c5user.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5witerconf.cpp \
    ../Classes/c5order.cpp \
    ../Classes/c5waiterserver.cpp \
    ../Classes/c5dishtabledelegate.cpp \
    ../Classes/c5ordertabledelegate.cpp \
    ../Classes/c5halltabledelegate.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printservicethread.cpp \
    dlgpayment.cpp \
    ../Classes/c5printreceiptthread.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../../NewTax/printtaxn.cpp \
    ../Controls/c5lineedit.cpp \
    ../Classes/c5printremovedservicethread.cpp

HEADERS += \
        dlgface.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5sockethandler.h \
    ../Cafe5/c5socketmessage.h \
    ../Cafe5/c5serverhandler.h \
    dlgpassword.h \
    dlgorder.h \
    ../Classes/c5user.h \
    ../Classes/c5menu.h \
    ../Classes/c5witerconf.h \
    ../Classes/c5order.h \
    ../Classes/c5waiterserver.h \
    ../Classes/c5dishtabledelegate.h \
    ../Classes/c5ordertabledelegate.h \
    ../Classes/c5halltabledelegate.h \
    ../Classes/c5printing.h \
    ../Classes/c5printservicethread.h \
    dlgpayment.h \
    ../Classes/c5printreceiptthread.h \
    ../Classes/QRCodeGenerator.h \
    ../../NewTax/printtaxn.h \
    ../Controls/c5lineedit.h \
    ../Classes/c5printremovedservicethread.h

FORMS += \
        dlgface.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    dlgpassword.ui \
    dlgorder.ui \
    dlgpayment.ui

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Controls
INCLUDEPATH += ../../NewTax
INCLUDEPATH += C:/OpenSSL-Win32.0/include
INCLUDEPATH += C:/OpenSSL-Win32.0/include/openssl

RESOURCES += \
    ../resources/resources.qrc \
    translator.qrc

DISTFILES +=

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto
