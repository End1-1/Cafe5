QT += sql gui core widgets network printsupport

CONFIG += dll

TEMPLATE = lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

RC_FILE = res.rc

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"WAITER\\\"
DEFINES += DLL

INCLUDEPATH += ../../Cafe5
INCLUDEPATH += ../../FrontDesk
INCLUDEPATH += ../../Classes
INCLUDEPATH += ../../Controls
INCLUDEPATH += c:/projects/xlsx/src

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../XLSX/src/xlsx.cpp \
    ../../../XLSX/src/xlsxcell.cpp \
    ../../../XLSX/src/xlsxcontenttype.cpp \
    ../../../XLSX/src/xlsxdocpropsapp.cpp \
    ../../../XLSX/src/xlsxdocpropscore.cpp \
    ../../../XLSX/src/xlsxdocument.cpp \
    ../../../XLSX/src/xlsxrels.cpp \
    ../../../XLSX/src/xlsxsharedstring.cpp \
    ../../../XLSX/src/xlsxsheet.cpp \
    ../../../XLSX/src/xlsxstyles.cpp \
    ../../../XLSX/src/xlsxtheme.cpp \
    ../../../XLSX/src/xlsxworkbook.cpp \
    ../../../XLSX/src/xlsxwriter.cpp \
    ../../Classes/notificationwidget.cpp \
    dll.cpp \
    ../../Cafe5/c5database.cpp \
    ../../Cafe5/c5message.cpp \
    ../../Cafe5/c5config.cpp \
    ../../Cafe5/c5utils.cpp \
    ../../Classes/c5printing.cpp \
    ../../Controls/c5checkbox.cpp \
    ../../Controls/c5combobox.cpp \
    ../../Controls/c5dateedit.cpp \
    ../../Controls/c5lineedit.cpp \
    ../../Classes/c5cache.cpp

HEADERS += \
    ../../../XLSX/src/crs32.h \
    ../../../XLSX/src/xlsx.h \
    ../../../XLSX/src/xlsxall.h \
    ../../../XLSX/src/xlsxcell.h \
    ../../../XLSX/src/xlsxcontenttype.h \
    ../../../XLSX/src/xlsxdocpropsapp.h \
    ../../../XLSX/src/xlsxdocpropscore.h \
    ../../../XLSX/src/xlsxdocument.h \
    ../../../XLSX/src/xlsxrels.h \
    ../../../XLSX/src/xlsxsharedstring.h \
    ../../../XLSX/src/xlsxsheet.h \
    ../../../XLSX/src/xlsxstyles.h \
    ../../../XLSX/src/xlsxtheme.h \
    ../../../XLSX/src/xlsxworkbook.h \
    ../../../XLSX/src/xlsxwriter.h \
    ../../../XLSX/src/zip.h \
    ../../Classes/notificationwidget.h \
    dll.h \
    ../../Cafe5/c5database.h \
    ../../Cafe5/c5message.h \
    ../../Cafe5/c5config.h \
    ../../Cafe5/c5utils.h \
    ../../Classes/c5printing.h \
    ../../Controls/c5checkbox.h \
    ../../Controls/c5combobox.h \
    ../../Controls/c5dateedit.h \
    ../../Controls/c5lineedit.h \
    ../../Classes/c5cache.h

FORMS += \
    ../../Cafe5/c5message.ui \
    ../../Classes/notificationwidget.ui

RESOURCES += \
    res.qrc
