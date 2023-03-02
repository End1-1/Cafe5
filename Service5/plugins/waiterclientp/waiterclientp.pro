QT += gui network sql printsupport

TEMPLATE = lib
DEFINES += WAITERCLIENTP_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../../NewTax/Src/printtaxn.cpp \
    ../../../Cafe5/c5config.cpp \
    ../../../Cafe5/c5database.cpp \
    ../../../Cafe5/c5logsystem.cpp \
    ../../../Cafe5/c5utils.cpp \
    ../../../Classes/QRCodeGenerator.cpp \
    ../../../Classes/c5printing.cpp \
    ../../../Classes/c5printreceiptthread.cpp \
    ../../../Classes/c5translator.cpp \
    ../../raw/rawmessage.cpp \
    ../../socket/sslsocket.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/databaseconnectionmanager.cpp \
    ../../utils/jsonhandler.cpp \
    ../../utils/logwriter.cpp \
    ../../utils/networktable.cpp \
    dishes.cpp \
    halls.cpp \
    login.cpp \
    ops.cpp \
    order.cpp \
    printbill.cpp \
    waiterclienthandler.cpp \
    waiterclientp.cpp \
    waiterconnection.cpp

HEADERS += \
    ../../../../NewTax/Src/printtaxn.h \
    ../../../Cafe5/c5config.h \
    ../../../Cafe5/c5database.h \
    ../../../Cafe5/c5logsystem.h \
    ../../../Cafe5/c5utils.h \
    ../../../Classes/QRCodeGenerator.h \
    ../../../Classes/c5printing.h \
    ../../../Classes/c5printreceiptthread.h \
    ../../../Classes/c5translator.h \
    ../../raw/rawmessage.h \
    ../../socket/sslsocket.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/databaseconnectionmanager.h \
    ../../utils/jsonhandler.h \
    ../../utils/logwriter.h \
    ../../utils/networktable.h \
    dishes.h \
    halls.h \
    login.h \
    ops.h \
    order.h \
    printbill.h \
    waiterclienthandler.h \
    waiterclientp.h \
    waiterconnection.h

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _ORGANIZATION_=\\\"BreezeDevs\\\"
DEFINES += _MODULE_=\\\"Service5_waiterclientp_dll\\\"
DEFINES += _NOAPP_

INCLUDEPATH += ../../raw
INCLUDEPATH += ../../socket
INCLUDEPATH += ../../utils
INCLUDEPATH += ../../../Classes
INCLUDEPATH += ../../../Cafe5

INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include/openssl
LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += -llibcrypto
LIBS += -lwsock32

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../../res/waiterclientp.qrc
