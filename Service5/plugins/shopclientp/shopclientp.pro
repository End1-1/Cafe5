QT += gui network sql printsupport

TEMPLATE = lib
DEFINES += SHOCLIENTP_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../../NewTax/Src/printtaxn.cpp \
    ../../../Classes/c5printing.cpp \
    ../../../Service5Working/raw/rawmessage.cpp \
    ../../socket/sslsocket.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/databaseconnectionmanager.cpp \
    ../../utils/jsonhandler.cpp \
    ../../utils/logwriter.cpp \
    ../../utils/networktable.cpp \
    datadriver.cpp \
    login.cpp \
    ops.cpp \
    shopclienthandler.cpp \
    shopclientp.cpp \
    shopconnection.cpp \
    sqdriver.cpp \
    store.cpp \
    traiding.cpp \
    translator.cpp

HEADERS += \
    ../../../../NewTax/Src/printtaxn.h \
    ../../../Classes/c5printing.h \
    ../../../Service5Working/raw/rawmessage.h \
    ../../socket/sslsocket.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/databaseconnectionmanager.h \
    ../../utils/jsonhandler.h \
    ../../utils/logwriter.h \
    ../../utils/networktable.h \
    datadriver.h \
    login.h \
    ops.h \
    shopclienthandler.h \
    shopclientp.h \
    shopconnection.h \
    sqdriver.h \
    store.h \
    traiding.h \
    translator.h

DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _MODULE_=\\\"Service5_shopclientp_dll\\\"

INCLUDEPATH += ../../raw
INCLUDEPATH += ../../socket
INCLUDEPATH += ../../utils
INCLUDEPATH += ../../../Classes

INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += C:/projects/Cafe5/Service5Working/raw
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
    ../../res/shopclientp.qrc
