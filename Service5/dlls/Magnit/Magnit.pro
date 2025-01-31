QT -= gui
QT += core sql

TEMPLATE = lib
DEFINES += MAGNIT_LIBRARY
DEFINES += BUILDING_MAGNIT_DLL

QMAKE_LFLAGS += /EXPORT:armsoft

CONFIG += c++17
CONFIG += dll exceptions
CONFIG += moc

SOURCES +=  \
    ../../../Classes/logwriter.cpp \
    magnit.cpp \
    ../../utils/database.cpp \
    armsoft.cpp \
    jsonreponse.cpp \
    locale.cpp

HEADERS += \
    ../../../Classes/logwriter.h \
    magnit.h \
    ../../utils/database.h \
    actions.h \
    armsoft.h \
    jsonreponse.h \
    locale.h

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

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
