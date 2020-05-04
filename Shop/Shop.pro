#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T11:27:26
#
#-------------------------------------------------

QT       += core gui printsupport network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Shop
TEMPLATE = app

RC_FILE = res.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
# disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../../XLSX/src/xlsx.cpp \
    ../../XLSX/src/xlsxcell.cpp \
    ../../XLSX/src/xlsxcontenttype.cpp \
    ../../XLSX/src/xlsxdocpropsapp.cpp \
    ../../XLSX/src/xlsxdocpropscore.cpp \
    ../../XLSX/src/xlsxdocument.cpp \
    ../../XLSX/src/xlsxrels.cpp \
    ../../XLSX/src/xlsxsharedstring.cpp \
    ../../XLSX/src/xlsxsheet.cpp \
    ../../XLSX/src/xlsxstyles.cpp \
    ../../XLSX/src/xlsxtheme.cpp \
    ../../XLSX/src/xlsxworkbook.cpp \
    ../../XLSX/src/xlsxwriter.cpp \
    ../Cafe5/c5license.cpp \
    ../Classes/c5crypt.cpp \
    ../Classes/c5grid.cpp \
    ../Classes/c5gridgilter.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5shoporder.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5widget.cpp \
    ../Classes/notificationwidget.cpp \
    ../Controls/c5graphicsview.cpp \
    ../Controls/c5lineeditwithselector.cpp \
    ../Editors/c5editor.cpp \
    ../Editors/ce5editor.cpp \
    ../FrontDesk/c5document.cpp \
    ../FrontDesk/c5filtervalues.cpp \
    ../FrontDesk/c5selector.cpp \
    c5replication.cpp \
    dlgpin.cpp \
        main.cpp \
    printreceipt.cpp \
    replicadialog.cpp \
    sales.cpp \
    settingsselection.cpp \
    storeinput.cpp \
    viewinputitem.cpp \
    vieworder.cpp \
        working.cpp \
    worder.cpp \
    goods.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5sockethandler.cpp \
    ../Controls/c5lineedit.cpp \
    ../Controls/c5tablewidget.cpp \
    ../Controls/c5combobox.cpp \
    ../Controls/c5dateedit.cpp \
    ../Classes/c5cache.cpp \
    ../../NewTax/Src/printtaxn.cpp \
    dqty.cpp \
    ../Controls/c5checkbox.cpp

HEADERS += \
    ../../XLSX/src/crs32.h \
    ../../XLSX/src/xlsx.h \
    ../../XLSX/src/xlsxall.h \
    ../../XLSX/src/xlsxcell.h \
    ../../XLSX/src/xlsxcontenttype.h \
    ../../XLSX/src/xlsxdocpropsapp.h \
    ../../XLSX/src/xlsxdocpropscore.h \
    ../../XLSX/src/xlsxdocument.h \
    ../../XLSX/src/xlsxrels.h \
    ../../XLSX/src/xlsxsharedstring.h \
    ../../XLSX/src/xlsxsheet.h \
    ../../XLSX/src/xlsxstyles.h \
    ../../XLSX/src/xlsxtheme.h \
    ../../XLSX/src/xlsxworkbook.h \
    ../../XLSX/src/xlsxwriter.h \
    ../../XLSX/src/zip.h \
    ../Cafe5/c5license.h \
    ../Classes/c5crypt.h \
    ../Classes/c5grid.h \
    ../Classes/c5gridgilter.h \
    ../Classes/c5printing.h \
    ../Classes/c5printpreview.h \
    ../Classes/c5shoporder.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5tablemodel.h \
    ../Classes/c5tableview.h \
    ../Classes/c5widget.h \
    ../Classes/notificationwidget.h \
    ../Controls/c5graphicsview.h \
    ../Controls/c5lineeditwithselector.h \
    ../Editors/c5editor.h \
    ../Editors/ce5editor.h \
    ../FrontDesk/c5document.h \
    ../FrontDesk/c5filtervalues.h \
    ../FrontDesk/c5selector.h \
    c5replication.h \
    dlgpin.h \
    printreceipt.h \
    replicadialog.h \
    sales.h \
    settingsselection.h \
    storeinput.h \
    viewinputitem.h \
    vieworder.h \
        working.h \
    worder.h \
    goods.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5sockethandler.h \
    ../Cafe5/c5socketmessage.h \
    ../Controls/c5lineedit.h \
    ../Controls/c5tablewidget.h \
    ../Controls/c5combobox.h \
    ../Controls/c5dateedit.h \
    ../Classes/c5cache.h \
    ../../NewTax/Src/printtaxn.h \
    dqty.h \
    ../Controls/c5checkbox.h

FORMS += \
    ../Classes/c5grid.ui \
    ../Classes/c5gridgilter.ui \
    ../Classes/c5printpreview.ui \
    ../Classes/notificationwidget.ui \
    ../Editors/c5editor.ui \
    ../Editors/ce5goods.ui \
    ../Editors/ce5goodsgroup.ui \
    ../Editors/ce5goodsunit.ui \
    ../Editors/ce5partner.ui \
    ../FrontDesk/c5filtervalues.ui \
    ../FrontDesk/c5selector.ui \
    dlgpin.ui \
    replicadialog.ui \
    sales.ui \
    settingsselection.ui \
    storeinput.ui \
    viewinputitem.ui \
    vieworder.ui \
        working.ui \
    worder.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    dqty.ui

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Hotelicca\\\"
DEFINES += _APPLICATION_=\\\"SmartHotel\\\"
DEFINES += _MODULE_=\\\"Shop\\\"
DEFINES += SHOP

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Classes
INCLUDEPATH += ../FrontDesk
INCLUDEPATH += ../Cache
INCLUDEPATH += ../Reports
INCLUDEPATH += ../Controls
INCLUDEPATH += ../Editors
INCLUDEPATH += ../RESOURCES
INCLUDEPATH += ../../XLSX/Src
INCLUDEPATH += C:/Projects/NewTax/Src


ICON = bag.ico

RESOURCES += \
    ../resources/res.qrc \
    local.qrc

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto
