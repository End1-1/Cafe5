QT -= gui
QT += sql

TEMPLATE = lib
DEFINES += JZSTORE_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../handlers/httpheader.cpp \
    ../../handlers/requesthandler.cpp \
    ../../handlers/socketdata.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/databaseconnectionmanager.cpp \
    ../../utils/jsonhandler.cpp \
    ../../utils/logwriter.cpp \
    jzstore.cpp

HEADERS += \
    ../../handlers/httpheader.h \
    ../../handlers/requesthandler.h \
    ../../handlers/socketdata.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/databaseconnectionmanager.h \
    ../../utils/jsonhandler.h \
    ../../utils/logwriter.h \
    jzstore.h

DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _MODULE_=\\\"Service5_jzstore_dll\\\"

TRANSLATIONS += \
    JZStore_hy_AM.ts

INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
