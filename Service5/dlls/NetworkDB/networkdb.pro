QT -= gui

QT += network printsupport

TEMPLATE = lib
DEFINES += SQLQUERY_LIBRARY
DEFINES += MYLIB_EXPORTS
TARGET = waiterservice

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../../NewTax/Src/printtaxn.cpp \
    ../../../Cafe5/c5utils.cpp \
    ../../../Classes/c5networkdb.cpp \
    ../../../Classes/c5printing.cpp \
    ../../../Classes/c5tr.cpp \
    queryjsonresponse.cpp \
    sqlquery.cpp

HEADERS += \
    ../../../../NewTax/Src/printtaxn.h \
    ../../../Cafe5/c5utils.h \
    ../../../Classes/c5networkdb.h \
    ../../../Classes/c5printing.h \
    ../../../Classes/c5tr.h \
    queryjsonresponse.h \
    sqlquery.h

SOURCES += \
    ../../utils/commandline.cpp \
    ../../utils/configini.cpp \
    ../../utils/logwriter.cpp \


HEADERS += \
    ../../utils/commandline.h \
    ../../utils/configini.h \
    ../../utils/logwriter.h \


INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Cafe5
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils
INCLUDEPATH += C:/projects/newtax/src
INCLUDEPATH += C:/Soft/OpenSSLWin64/include
INCLUDEPATH += C:/Soft/OpenSSLWin64/include/openssl

DEFINES += _APPLICATION_=\\\"VIPClient\\\"
DEFINES += _MODULE_=\\\"Service5_vipclient_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"

LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += -llibcrypto
LIBS += -lVersion
LIBS += -lwsock32

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
