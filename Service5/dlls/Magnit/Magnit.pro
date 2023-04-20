QT -= gui
QT += sql

TEMPLATE = lib
DEFINES += MAGNIT_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../handlers/httpheader.cpp \
    ../../handlers/requesthandler.cpp \
    ../../handlers/socketdata.cpp \
    ../../utils/commandline.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/jsonhandler.cpp \
    ../../utils/logwriter.cpp \
    armsoft.cpp \
    jsonreponse.cpp \
    locale.cpp \
    magnit.cpp

HEADERS += \
    ../../handlers/httpheader.h \
    ../../handlers/requesthandler.h \
    ../../handlers/socketdata.h \
    ../../utils/commandline.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/jsonhandler.h \
    ../../utils/logwriter.h \
    actions.h \
    armsoft.h \
    jsonreponse.h \
    locale.h \
    magnit.h

INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils

DEFINES += _APPLICATION_=\\\"Magnit\\\"
DEFINES += _MODULE_=\\\"Service5_magnit_dll\\\"

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
