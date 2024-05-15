QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

RC_FILE = res.rc

SOURCES += \
    ../Classes/c5crypt.cpp \
    ../Classes/thread.cpp \
    ../Service5Working/raw/raw.cpp \
    ../Service5Working/raw/rawbalancehistory.cpp \
    ../Service5Working/raw/rawcarnear.cpp \
    ../Service5Working/raw/rawcoordinate.cpp \
    ../Service5Working/raw/rawdllop.cpp \
    ../Service5Working/raw/rawhandler.cpp \
    ../Service5Working/raw/rawhello.cpp \
    ../Service5Working/raw/rawmessage.cpp \
    ../Service5Working/raw/rawmonitor.cpp \
    ../Service5Working/raw/rawplugin.cpp \
    ../Service5Working/raw/rawpluginobject.cpp \
    ../Service5Working/raw/rawregisterphone.cpp \
    ../Service5Working/raw/rawregistersms.cpp \
    ../Service5Working/raw/rawsilentauth.cpp \
    ../Service5Working/raw/rawyandexkey.cpp \
    ../Service5Working/socket/sslserver.cpp \
    ../Service5Working/socket/sslsocket.cpp \
    ../Service5Working/utils/commandline.cpp \
    ../Service5Working/utils/configini.cpp \
    ../Service5Working/utils/database.cpp \
    ../Service5Working/utils/databaseconnectionmanager.cpp \
    ../Service5Working/utils/gtranslator.cpp \
    ../Service5Working/utils/jsonhandler.cpp \
    ../Service5Working/utils/logwriter.cpp \
    ../Service5Working/utils/networktable.cpp \
    ../Service5Working/utils/os.cpp \
    ../Service5Working/utils/sqlqueries.cpp \
    ../Service5Working/utils/storemovement.cpp \
    ../Service5Working/utils/tablerecord.cpp \
    c5license.cpp \
    pluginmanager.cpp \
    utils/commandline.cpp \
    utils/configini.cpp \
    utils/database.cpp \
    datadriver/datadriver.cpp \
    dlglicenses.cpp \
    firebase.cpp \
    handlers/requesthandler.cpp \
    handlers/requestmanager.cpp \
    handlers/httpheader.cpp \
    utils/jsonhandler.cpp \
    main.cpp \
    monitor.cpp \
    serverthread.cpp \
    socket/sslserver.cpp \
    socket/sslsocket.cpp \
    handlers/socketdata.cpp \
    socketthread.cpp \
    threadworker.cpp \
    utils/logwriter.cpp \
    utils/os.cpp \
    utils/sqlqueries.cpp

HEADERS += \
    ../Classes/c5crypt.h \
    ../Classes/thread.h \
    ../Service5Working/raw/messagelist.h \
    ../Service5Working/raw/raw.h \
    ../Service5Working/raw/rawbalancehistory.h \
    ../Service5Working/raw/rawcarnear.h \
    ../Service5Working/raw/rawcoordinate.h \
    ../Service5Working/raw/rawdllop.h \
    ../Service5Working/raw/rawhandler.h \
    ../Service5Working/raw/rawhello.h \
    ../Service5Working/raw/rawmessage.h \
    ../Service5Working/raw/rawmonitor.h \
    ../Service5Working/raw/rawplugin.h \
    ../Service5Working/raw/rawpluginobject.h \
    ../Service5Working/raw/rawregisterphone.h \
    ../Service5Working/raw/rawregistersms.h \
    ../Service5Working/raw/rawsilentauth.h \
    ../Service5Working/raw/rawyandexkey.h \
    ../Service5Working/raw/structs.h \
    ../Service5Working/socket/sslserver.h \
    ../Service5Working/socket/sslsocket.h \
    ../Service5Working/utils/commandline.h \
    ../Service5Working/utils/configini.h \
    ../Service5Working/utils/database.h \
    ../Service5Working/utils/databaseconnectionmanager.h \
    ../Service5Working/utils/gtranslator.h \
    ../Service5Working/utils/jsonhandler.h \
    ../Service5Working/utils/logwriter.h \
    ../Service5Working/utils/networktable.h \
    ../Service5Working/utils/os.h \
    ../Service5Working/utils/sqlqueries.h \
    ../Service5Working/utils/storemovement.h \
    ../Service5Working/utils/tablerecord.h \
    c5license.h \
    pluginmanager.h \
    utils/commandline.h \
    utils/configini.h \
    utils/database.h \
    datadriver/datadriver.h \
    dlglicenses.h \
    firebase.h \
    handlers/requesthandler.h \
    handlers/requestmanager.h \
    handlers/httpheader.h \
    utils/jsonhandler.h \
    monitor.h \
    rc.h \
    serverthread.h \
    socket/sslserver.h \
    socket/sslsocket.h \
    handlers/socketdata.h \
    socketthread.h \
    threadworker.h \
    utils/logwriter.h \
    utils/os.h \
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


