QT       += core gui network sql websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

RC_FILE = res.rc

SOURCES += \
    ../Classes/c5crypt.cpp \
    ../Classes/thread.cpp \
    ../Service5Working/utils/commandline.cpp \
    ../Service5Working/utils/configini.cpp \
    ../Service5Working/utils/database.cpp \
    ../Service5Working/utils/gtranslator.cpp \
    ../Service5Working/utils/logwriter.cpp \
    ../Service5Working/utils/tablerecord.cpp \
    c5searchengine.cpp \
    os.cpp \
    utils/commandline.cpp \
    utils/configini.cpp \
    utils/database.cpp \
    datadriver/datadriver.cpp \
    dlglicenses.cpp \
    firebase.cpp \
    handlers/httpheader.cpp \
    main.cpp \
    monitor.cpp \
    serverthread.cpp \
    utils/logwriter.cpp \
    utils/sqlqueries.cpp

HEADERS += \
    ../Classes/c5crypt.h \
    ../Classes/thread.h \
    ../Service5Working/utils/commandline.h \
    ../Service5Working/utils/configini.h \
    ../Service5Working/utils/database.h \
    ../Service5Working/utils/gtranslator.h \
    ../Service5Working/utils/logwriter.h \
    ../Service5Working/utils/tablerecord.h \
    c5searchengine.h \
    os.h \
    utils/commandline.h \
    utils/configini.h \
    utils/database.h \
    datadriver/datadriver.h \
    dlglicenses.h \
    firebase.h \
    handlers/httpheader.h \
    monitor.h \
    rc.h \
    serverthread.h \
    utils/logwriter.h \
    utils/sqlqueries.h

FORMS += \
    ../Cafe5/c5message.ui \
    dlglicenses.ui \
    monitor.ui

INCLUDEPATH += handlers
INCLUDEPATH += headers
INCLUDEPATH += store
INCLUDEPATH += datadriver
INCLUDEPATH += ../Service5Working/raw
INCLUDEPATH += ../Service5Working/socket
INCLUDEPATH += ../Classes
INCLUDEPATH += C:/projects/Cafe5/Service5Working/utils
INCLUDEPATH += C:/Soft/OpenSSLWin64/include
INCLUDEPATH += C:/Soft/OpenSSLWin64/include/openssl

DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += _ORGANIZATION_=\\\"BreezeDevs\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"Service5\\\"
DEFINES += QSSLSOCKET_DEBUG

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += advapi32.lib
LIBS += -llibcrypto

QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:"level='requireAdministrator'"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


