QT += network sql

TEMPLATE = lib
DEFINES += SYNCFROMAWS_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../Classes/c5filelogwriter.cpp \
    ../../../Classes/c5threadobject.cpp \
    ../../../NTable/ndataprovider.cpp \
    syncfromaws.cpp \
    ../../../Service5Working/raw/rawmessage.cpp \
    ../../socket/sslsocket.cpp \
    ../../utils/database.cpp \
    ../../utils/logwriter.cpp \
    worker.cpp

HEADERS += \
    ../../../Classes/c5filelogwriter.h \
    ../../../Classes/c5threadobject.h \
    ../../../NTable/ndataprovider.h \
    syncfromaws.h \
    ../../../Service5Working/raw/rawmessage.h \
    ../../socket/sslsocket.h \
    ../../utils/database.h \
    ../../utils/logwriter.h \
    worker.h

DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _MODULE_=\\\"Service5_syncfromaws_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += VALSHIN

INCLUDEPATH += ../../raw
INCLUDEPATH += ../../socket
INCLUDEPATH += ../../utils
INCLUDEPATH += ../../../Classes
INCLUDEPATH += ../../../NTable
INCLUDEPATH += ../../../Service5Working/raw

INCLUDEPATH += C:/Soft/OpenSSLWin64/include
INCLUDEPATH += C:/Soft/OpenSSLWin64/include/openssl
LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += -llibcrypto
LIBS += -lwsock32

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

FORMS +=
