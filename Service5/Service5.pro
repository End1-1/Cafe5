QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_FILE = res.rc

SOURCES += \
    ../Classes/c5crypt.cpp \
    ../Classes/thread.cpp \
    c5license.cpp \
    pluginmanager.cpp \
    raw/rawdllop.cpp \
    raw/rawplugin.cpp \
    raw/rawpluginobject.cpp \
    utils/commandline.cpp \
    utils/configini.cpp \
    utils/database.cpp \
    utils/databaseconnectionmanager.cpp \
    datadriver/datadriver.cpp \
    dlglicenses.cpp \
    firebase.cpp \
    handlers/requesthandler.cpp \
    handlers/requestmanager.cpp \
    handlers/httpheader.cpp \
    utils/jsonhandler.cpp \
    main.cpp \
    monitor.cpp \
    raw/raw.cpp \
    raw/rawbalancehistory.cpp \
    raw/rawcarnear.cpp \
    raw/rawcoordinate.cpp \
    raw/rawhandler.cpp \
    raw/rawhello.cpp \
    raw/rawmessage.cpp \
    raw/rawmonitor.cpp \
    raw/rawregisterphone.cpp \
    raw/rawregistersms.cpp \
    raw/rawsilentauth.cpp \
    raw/rawyandexkey.cpp \
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
    c5license.h \
    pluginmanager.h \
    raw/rawdllop.h \
    raw/rawplugin.h \
    raw/rawpluginobject.h \
    utils/commandline.h \
    utils/configini.h \
    utils/database.h \
    utils/databaseconnectionmanager.h \
    datadriver/datadriver.h \
    dlglicenses.h \
    firebase.h \
    handlers/requesthandler.h \
    handlers/requestmanager.h \
    handlers/httpheader.h \
    utils/jsonhandler.h \
    monitor.h \
    raw/messagelist.h \
    raw/raw.h \
    raw/rawbalancehistory.h \
    raw/rawcarnear.h \
    raw/rawcoordinate.h \
    raw/rawhandler.h \
    raw/rawhello.h \
    raw/rawmessage.h \
    raw/rawmonitor.h \
    raw/rawregisterphone.h \
    raw/rawregistersms.h \
    raw/rawsilentauth.h \
    raw/rawyandexkey.h \
    raw/structs.h \
    rc.h \
    res.rc \
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

INCLUDEPATH += socket
INCLUDEPATH += utils
INCLUDEPATH += handlers
INCLUDEPATH += headers
INCLUDEPATH += store
INCLUDEPATH += datadriver
INCLUDEPATH += raw
INCLUDEPATH += ../Classes
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include/openssl

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"BreezeDevs\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"Service5\\\"

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += advapi32.lib
LIBS += -llibcrypto

QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:"level='requireAdministrator'"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
