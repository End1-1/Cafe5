QT -= gui
QT += sql

TEMPLATE = lib
DEFINES += MAGNIT_LIBRARY
DEFINES += BUILDING_MAGNIT_DLL

QMAKE_LFLAGS += /EXPORT:armsoft

CONFIG += c++11
CONGIF += dll

SOURCES += \
    magnit.cpp

HEADERS += \
    magnit.h

SOURCES +=  \
    ../../handlers/httpheader.cpp \
    ../../utils/commandline.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/logwriter.cpp \
    armsoft.cpp \
    jsonreponse.cpp \
    locale.cpp

HEADERS += \
    ../../handlers/httpheader.h \
    ../../utils/commandline.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/logwriter.h \
    actions.h \
    armsoft.h \
    jsonreponse.h \
    locale.h

INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils

DEFINES += _APPLICATION_=\\\"Magnit\\\"
DEFINES += _MODULE_=\\\"Service5_magnit_dll\\\"
DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
