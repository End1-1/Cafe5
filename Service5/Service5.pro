QT       += core gui network sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_FILE = res.rc

SOURCES += \
    ../../NewTax/Src/printtax.cpp \
    ../../NewTax/Src/printtaxn.cpp \
    configini.cpp \
    database.cpp \
    databaseconnectionmanager.cpp \
    datadriver/datadriver.cpp \
    datadriver/dduser.cpp \
    firebase.cpp \
    handlers/authentication.cpp \
    handlers/chat.cpp \
    handlers/confirmregistration.cpp \
    handlers/forbidden.cpp \
    handlers/jzstore.cpp \
    handlers/notfound.cpp \
    handlers/registration.cpp \
    handlers/requesthandler.cpp \
    handlers/requestmanager.cpp \
    handlers/shoprequest.cpp \
    handlers/storerequest.cpp \
    handlers/tax.cpp \
    httpheader.cpp \
    jsonhandler.cpp \
    main.cpp \
    monitor.cpp \
    raw/raw.cpp \
    raw/rawbalancehistory.cpp \
    raw/rawcoordinate.cpp \
    raw/rawdataexchange.cpp \
    raw/rawhandler.cpp \
    raw/rawhello.cpp \
    raw/rawregisterphone.cpp \
    raw/rawregistersms.cpp \
    raw/rawsilentauth.cpp \
    raw/rawyandexkey.cpp \
    serverthread.cpp \
    socket/sslserver.cpp \
    socket/sslsocket.cpp \
    socketdata.cpp \
    socketthread.cpp \
    store/shopmanager.cpp \
    store/storemanager.cpp \
    thread.cpp \
    threadworker.cpp \
    utils/c5printing.cpp \
    utils/logwriter.cpp \
    utils/printreceiptgroup.cpp

HEADERS += \
    ../../NewTax/Src/printtax.h \
    ../../NewTax/Src/printtaxn.h \
    configini.h \
    database.h \
    databaseconnectionmanager.h \
    datadriver/datadriver.h \
    datadriver/dduser.h \
    firebase.h \
    handlers/authentication.h \
    handlers/chat.h \
    handlers/confirmregistration.h \
    handlers/forbidden.h \
    handlers/jzstore.h \
    handlers/notfound.h \
    handlers/registration.h \
    handlers/requesthandler.h \
    handlers/requestmanager.h \
    handlers/shoprequest.h \
    handlers/storerequest.h \
    handlers/tax.h \
    headers/registrationstate.h \
    httpheader.h \
    jsonhandler.h \
    monitor.h \
    raw/messagelist.h \
    raw/raw.h \
    raw/rawbalancehistory.h \
    raw/rawcoordinate.h \
    raw/rawdataexchange.h \
    raw/rawhandler.h \
    raw/rawhello.h \
    raw/rawregisterphone.h \
    raw/rawregistersms.h \
    raw/rawsilentauth.h \
    raw/rawyandexkey.h \
    rc.h \
    res.rc \
    serverthread.h \
    socket/sslserver.h \
    socket/sslsocket.h \
    socketdata.h \
    socketthread.h \
    store/shopmanager.h \
    store/storemanager.h \
    thread.h \
    threadworker.h \
    utils/c5printing.h \
    utils/logwriter.h \
    utils/printreceiptgroup.h

FORMS += \
    ../Cafe5/c5message.ui \
    monitor.ui

INCLUDEPATH += socket
INCLUDEPATH += utils
INCLUDEPATH += handlers
INCLUDEPATH += headers
INCLUDEPATH += store
INCLUDEPATH += datadriver
INCLUDEPATH += raw
INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include/openssl

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Hotelicca\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"Service5\\\"
DEFINES += SHOP

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += advapi32.lib
LIBS += -llibcrypto

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
