#-------------------------------------------------
#
# Project created by QtCreator 2018-10-26T15:29:15
#
#-------------------------------------------------

QT       += core gui network printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Smart
TEMPLATE = app

RC_FILE = res.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += WAITER

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../../NewTax/Src/printtaxn.cpp \
    ../../XLSX/src/xlsx.cpp \
    ../../XLSX/src/xlsxborder.cpp \
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
    ../Cafe5/c5cafecommon.cpp \
    ../Cafe5/c5double.cpp \
    ../Cafe5/c5logsystem.cpp \
    ../Cafe5/c5serverhandler.cpp \
    ../Cafe5/c5systempreference.cpp \
    ../Cafe5/calendar.cpp \
    ../Cafe5/dlgexitbyversion.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../Classes/c5cache.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5crypt.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Classes/c5filelogwriter.cpp \
    ../Classes/c5gridgilter.cpp \
    ../Classes/c5halltabledelegate.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5logtoserverthread.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5permissions.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printjson.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5printreceiptthread.cpp \
    ../Classes/c5printreceiptthread50mm.cpp \
    ../Classes/c5printremovedservicethread.cpp \
    ../Classes/c5printservicethread.cpp \
    ../Classes/c5random.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5tablerec.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5textdelegate.cpp \
    ../Classes/c5threadobject.cpp \
    ../Classes/c5translator.cpp \
    ../Classes/c5waiterorderdoc.cpp \
    ../Classes/c5widget.cpp \
    ../Classes/c5witerconf.cpp \
    ../Classes/calculator.cpp \
    ../Classes/checkforupdatethread.cpp \
    ../Classes/dlgpassword.cpp \
    ../Classes/doubledatabase.cpp \
    ../Classes/fileversion.cpp \
    ../Classes/idram.cpp \
    ../Classes/notificationwidget.cpp \
    ../Classes/rkeyboard.cpp \
    ../Classes/thread.cpp \
    ../Classes/threadsendmessage.cpp \
    ../Controls/c5checkbox.cpp \
    ../Controls/c5combobox.cpp \
    ../Controls/c5dateedit.cpp \
    ../Controls/c5graphicsview.cpp \
    ../Controls/c5guicontrols.cpp \
    ../DbData/datadriver.cpp \
    ../DbData/dbbodystate.cpp \
    ../DbData/dbcar.cpp \
    ../DbData/dbdata.cpp \
    ../DbData/dbdishcomments.cpp \
    ../DbData/dbdishes.cpp \
    ../DbData/dbdishpart1.cpp \
    ../DbData/dbdishpart2.cpp \
    ../DbData/dbdishremovereason.cpp \
    ../DbData/dbdishspecial.cpp \
    ../DbData/dbgoods.cpp \
    ../DbData/dbgoodsgroup.cpp \
    ../DbData/dbhalls.cpp \
    ../DbData/dbmenu.cpp \
    ../DbData/dbmenuname.cpp \
    ../DbData/dbmenupackagelist.cpp \
    ../DbData/dbmenupackages.cpp \
    ../DbData/dboheader.cpp \
    ../DbData/dbopreorder.cpp \
    ../DbData/dbpartner.cpp \
    ../DbData/dbservicevalues.cpp \
    ../DbData/dbshift.cpp \
    ../DbData/dbstore.cpp \
    ../DbData/dbstoredoctype.cpp \
    ../DbData/dbtables.cpp \
    ../DbData/dbunit.cpp \
    ../DbData/dbusers.cpp \
    ../Forms/change.cpp \
    ../Forms/customerinfo.cpp \
    ../Forms/dlgqty.cpp \
    ../Forms/touchdlgphonenumber.cpp \
    ../Forms/touchentertaxreceiptnumber.cpp \
    ../Forms/touchselecttaxreport.cpp \
    ../Shop/printreceipt.cpp \
    ../Shop/printreceiptgroup.cpp \
    ../Shop/selectprinters.cpp \
    dishpackage.cpp \
        main.cpp \
    menudialog.cpp \
    menudish.cpp \
    orderdish.cpp \
    supplier.cpp \
        workspace.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5sockethandler.cpp \
    ../Controls/c5lineedit.cpp \
    ../Classes/c5user.cpp

HEADERS += \
    ../../NewTax/Src/printtaxn.h \
    ../../XLSX/src/crs32.h \
    ../../XLSX/src/xlsx.h \
    ../../XLSX/src/xlsxall.h \
    ../../XLSX/src/xlsxborder.h \
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
    ../Cafe5/c5cafecommon.h \
    ../Cafe5/c5double.h \
    ../Cafe5/c5logsystem.h \
    ../Cafe5/c5serverhandler.h \
    ../Cafe5/c5systempreference.h \
    ../Cafe5/calendar.h \
    ../Cafe5/dlgexitbyversion.h \
    ../Classes/QRCodeGenerator.h \
    ../Classes/barcode5.h \
    ../Classes/c5cache.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5crypt.h \
    ../Classes/c5datedelegate.h \
    ../Classes/c5filelogwriter.h \
    ../Classes/c5gridgilter.h \
    ../Classes/c5halltabledelegate.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5logtoserverthread.h \
    ../Classes/c5menu.h \
    ../Classes/c5permissions.h \
    ../Classes/c5printing.h \
    ../Classes/c5printjson.h \
    ../Classes/c5printpreview.h \
    ../Classes/c5printreceiptthread.h \
    ../Classes/c5printreceiptthread50mm.h \
    ../Classes/c5printremovedservicethread.h \
    ../Classes/c5printservicethread.h \
    ../Classes/c5random.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5tablemodel.h \
    ../Classes/c5tablerec.h \
    ../Classes/c5tableview.h \
    ../Classes/c5textdelegate.h \
    ../Classes/c5threadobject.h \
    ../Classes/c5translator.h \
    ../Classes/c5waiterorderdoc.h \
    ../Classes/c5widget.h \
    ../Classes/c5witerconf.h \
    ../Classes/calculator.h \
    ../Classes/checkforupdatethread.h \
    ../Classes/dlgpassword.h \
    ../Classes/doubledatabase.h \
    ../Classes/fileversion.h \
    ../Classes/idram.h \
    ../Classes/notificationwidget.h \
    ../Classes/rkeyboard.h \
    ../Classes/thread.h \
    ../Classes/threadsendmessage.h \
    ../Controls/c5checkbox.h \
    ../Controls/c5combobox.h \
    ../Controls/c5dateedit.h \
    ../Controls/c5graphicsview.h \
    ../Controls/c5guicontrols.h \
    ../DbData/datadriver.h \
    ../DbData/dbbodystate.h \
    ../DbData/dbcar.h \
    ../DbData/dbdata.h \
    ../DbData/dbdishcomments.h \
    ../DbData/dbdishes.h \
    ../DbData/dbdishpart1.h \
    ../DbData/dbdishpart2.h \
    ../DbData/dbdishremovereason.h \
    ../DbData/dbdishspecial.h \
    ../DbData/dbgoods.h \
    ../DbData/dbgoodsgroup.h \
    ../DbData/dbhalls.h \
    ../DbData/dbmenu.h \
    ../DbData/dbmenuname.h \
    ../DbData/dbmenupackagelist.h \
    ../DbData/dbmenupackages.h \
    ../DbData/dboheader.h \
    ../DbData/dbopreorder.h \
    ../DbData/dbpartner.h \
    ../DbData/dbservicevalues.h \
    ../DbData/dbshift.h \
    ../DbData/dbstore.h \
    ../DbData/dbstoredoctype.h \
    ../DbData/dbtables.h \
    ../DbData/dbunit.h \
    ../DbData/dbusers.h \
    ../Forms/change.h \
    ../Forms/customerinfo.h \
    ../Forms/dlgqty.h \
    ../Forms/touchdlgphonenumber.h \
    ../Forms/touchentertaxreceiptnumber.h \
    ../Forms/touchselecttaxreport.h \
    ../Service/servicecommands.h \
    ../Shop/printreceipt.h \
    ../Shop/printreceiptgroup.h \
    ../Shop/selectprinters.h \
    dish.h \
    dishpackage.h \
    menudialog.h \
    menudish.h \
    orderdish.h \
    rc.h \
    res.rc \
    supplier.h \
        workspace.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5sockethandler.h \
    ../Cafe5/c5socketmessage.h \
    ../Controls/c5lineedit.h \
    ../Classes/c5user.h

FORMS += \
    ../Cafe5/calendar.ui \
    ../Cafe5/dlgexitbyversion.ui \
    ../Classes/c5gridgilter.ui \
    ../Classes/c5printpreview.ui \
    ../Classes/calculator.ui \
    ../Classes/dlgpassword.ui \
    ../Classes/notificationwidget.ui \
    ../Classes/rkeyboard.ui \
    ../Forms/change.ui \
    ../Forms/customerinfo.ui \
    ../Forms/dlgqty.ui \
    ../Forms/touchdlgphonenumber.ui \
    ../Forms/touchentertaxreceiptnumber.ui \
    ../Forms/touchselecttaxreport.ui \
    ../Shop/selectprinters.ui \
    menudialog.ui \
    menudish.ui \
    orderdish.ui \
    supplier.ui \
        workspace.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"SMART\\\"
DEFINES += WAITER

INCLUDEPATH += c:/projects/cafe5/Cafe5
INCLUDEPATH += c:/projects/cafe5/Classes
INCLUDEPATH += c:/projects/cafe5/Controls
INCLUDEPATH += c:/projects/cafe5/Service
INCLUDEPATH += c:/projects/cafe5/DbData
INCLUDEPATH += c:/projects/cafe5/Forms
INCLUDEPATH += c:/Projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include/openssl
INCLUDEPATH += /projects/xlsx/src

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += -llibcrypto

RESOURCES += \
    ../resources/resources.qrc \
    transres.qrc
