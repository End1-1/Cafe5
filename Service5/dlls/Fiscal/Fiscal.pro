QT -= gui

QT += sql network printsupport

TEMPLATE = lib
DEFINES += SQLQUERY_LIBRARY

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
    ../../../Service5Working/utils/logwriter.cpp \
    sqlquery.cpp

HEADERS += \
    ../../../../NewTax/Src/printtaxn.h \
    ../../../Cafe5/c5utils.h \
    ../../../Classes/c5networkdb.h \
    ../../../Classes/c5printing.h \
    ../../../Classes/c5tr.h \
    ../../../Service5Working/utils/logwriter.h \
    sqlquery.h

SOURCES += \
    ../../handlers/httpheader.cpp \
    ../../utils/commandline.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/logwriter.cpp


HEADERS += \
    ../../handlers/httpheader.h \
    ../../utils/commandline.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/logwriter.h


INCLUDEPATH += C:/Projects/Cafe5/Cafe5
INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils
INCLUDEPATH += C:/Soft/OpenSSLWin64/include
INCLUDEPATH += C:/Soft/OpenSSLWin64/include/openssl
INCLUDEPATH += c:/Projects/NewTax/Src
INCLUDEPATH += c:/projects/cafe5/service5working/utils

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += -llibcrypto

DEFINES += _APPLICATION_=\\\"Picasso\\\"
DEFINES += _MODULE_=\\\"Service5_Fiscal_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += REMOTE_ALL

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
