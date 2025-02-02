QT -= gui
QT += sql

TEMPLATE = lib
DEFINES += JZSTORE_LIBRARY
TARGET = jzstore

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../Classes/logwriter.cpp \
    ../../utils/database.cpp \
    jzstore.cpp

HEADERS += \
    ../../../Classes/logwriter.h \
    ../../utils/database.h \
    jzstore.h

DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _MODULE_=\\\"Service5_jzstore_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"

TRANSLATIONS += \
    JZStore_hy_AM.ts

INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
