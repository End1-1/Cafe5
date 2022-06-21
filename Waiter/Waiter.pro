#-------------------------------------------------
#
# Project created by QtCreator 2018-06-21T10:59:55
#
#-------------------------------------------------

QT       += core gui network printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Waiter
TEMPLATE = app

RC_FILE = res.rc

ICON = cup.ico

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"WAITER\\\"
DEFINES += WAITER
# DEFINES += LOGGING

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
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
    ../Cafe5/c5systempreference.cpp \
    ../Cafe5/calendar.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5crypt.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Classes/c5document.cpp \
    ../Classes/c5filelogwriter.cpp \
    ../Classes/c5gridgilter.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5orderdriver.cpp \
    ../Classes/c5printreceiptthread.cpp \
    ../Classes/c5printreceiptthread50mm.cpp \
    ../Classes/c5random.cpp \
    ../Classes/c5shoporder.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5tablerec.cpp \
    ../Classes/c5textdelegate.cpp \
    ../Classes/c5threadobject.cpp \
    ../Classes/c5waiterorderdoc.cpp \
    ../Classes/calculator.cpp \
    ../Classes/checkforupdatethread.cpp \
    ../Classes/dlgpassword.cpp \
    ../Classes/doubledatabase.cpp \
    ../Classes/idram.cpp \
    ../Classes/notificationwidget.cpp \
    ../Classes/proxytablewidgetdatabase.cpp \
    ../Classes/threadsendmessage.cpp \
    ../Controls/c5guicontrols.cpp \
    ../Controls/c5tablewidget.cpp \
    ../DbData/datadriver.cpp \
    ../DbData/dbbodystate.cpp \
    ../DbData/dbcar.cpp \
    ../DbData/dbdata.cpp \
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
    ../Forms/c5filtervalues.cpp \
    ../Forms/change.cpp \
    ../Forms/dlglistofdishcomments.cpp \
    ../Forms/dlgqty.cpp \
    ../Shop/printreceipt.cpp \
    ../Shop/printreceiptgroup.cpp \
    ../Shop/selectprinters.cpp \
    dishitem.cpp \
    dishitembutton.cpp \
    dlgaskforprecheck.cpp \
    dlgcarnumber.cpp \
    dlgchosesplitorderoption.cpp \
    dlgguests.cpp \
    dlglist.cpp \
    dlglistdishspecial.cpp \
    dlglistofpackages.cpp \
    dlglistofshifts.cpp \
    dlgmanagertools.cpp \
    dlgprecheckoptions.cpp \
    dlgpreorder.cpp \
    dlgscreen.cpp \
    dlgshiftrotation.cpp \
    dlgsplitorder.cpp \
    dlgstafflist.cpp \
    dlgstoplistoption.cpp \
    dlgsystem.cpp \
    dlgtext.cpp \
    dlgtimeorder.cpp \
    dlgviewstoplist.cpp \
        main.cpp \
        dlgface.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5sockethandler.cpp \
    ../Cafe5/c5serverhandler.cpp \
    dlgorder.cpp \
    ../Classes/c5user.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5witerconf.cpp \
    ../Classes/c5waiterserver.cpp \
    ../Classes/c5ordertabledelegate.cpp \
    ../Classes/c5halltabledelegate.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printservicethread.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../../NewTax/Src/printtaxn.cpp \
    ../Controls/c5lineedit.cpp \
    ../Classes/c5printremovedservicethread.cpp \
    ../Controls/c5checkbox.cpp \
    ../Controls/c5combobox.cpp \
    ../Controls/c5dateedit.cpp \
    ../Controls/c5graphicsview.cpp \
    ../Classes/c5cache.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5widget.cpp \
    qdishbutton.cpp \
    qdishpart2button.cpp \
    tablewidget.cpp \
    tablewidgetv1.cpp \
    tablewidgetv2.cpp \
    wguestorder.cpp \
    worder.cpp \
    wpaymentoptions.cpp \
    dlgcreditcardlist.cpp \
    dlgdishremovereason.cpp \
    dlglistofhall.cpp \
    ../Classes/c5printjson.cpp \
    dlgreports.cpp \
    dlgreportslist.cpp \
    dlglistofmenu.cpp \
    dlgguest.cpp \
    ../Classes/rkeyboard.cpp \
    dlgreceiptlanguage.cpp \
    dlgcl.cpp \
    ../Classes/c5logtoserverthread.cpp \
    ../Classes/c5permissions.cpp \
    dlgexitwithmessage.cpp \
    ../Classes/c5translator.cpp \
    ../Cafe5/dlgexitbyversion.cpp \
    ../Classes/fileversion.cpp \
    dlgsearchinmenu.cpp

HEADERS += \
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
    ../Cafe5/c5systempreference.h \
    ../Cafe5/calendar.h \
    ../Classes/barcode5.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5crypt.h \
    ../Classes/c5datedelegate.h \
    ../Classes/c5document.h \
    ../Classes/c5filelogwriter.h \
    ../Classes/c5gridgilter.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5orderdriver.h \
    ../Classes/c5printreceiptthread.h \
    ../Classes/c5printreceiptthread50mm.h \
    ../Classes/c5random.h \
    ../Classes/c5shoporder.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5tablerec.h \
    ../Classes/c5textdelegate.h \
    ../Classes/c5threadobject.h \
    ../Classes/c5waiterorderdoc.h \
    ../Classes/calculator.h \
    ../Classes/checkforupdatethread.h \
    ../Classes/dlgpassword.h \
    ../Classes/doubledatabase.h \
    ../Classes/idram.h \
    ../Classes/notificationwidget.h \
    ../Classes/proxytablewidgetdatabase.h \
    ../Classes/threadsendmessage.h \
    ../Controls/c5guicontrols.h \
    ../Controls/c5tablewidget.h \
    ../DbData/datadriver.h \
    ../DbData/dbbodystate.h \
    ../DbData/dbcar.h \
    ../DbData/dbdata.h \
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
    ../Forms/c5filtervalues.h \
    ../Forms/change.h \
    ../Forms/dlglistofdishcomments.h \
    ../Forms/dlgqty.h \
    ../Service/servicecommands.h \
    ../Shop/printreceipt.h \
    ../Shop/printreceiptgroup.h \
    ../Shop/selectprinters.h \
    c5printreceiptthread50mm.h \
    dishitem.h \
    dishitembutton.h \
    dlgaskforprecheck.h \
    dlgcarnumber.h \
    dlgchosesplitorderoption.h \
        dlgface.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5sockethandler.h \
    ../Cafe5/c5socketmessage.h \
    ../Cafe5/c5serverhandler.h \
    dlgguests.h \
    dlglist.h \
    dlglistdishspecial.h \
    dlglistofpackages.h \
    dlglistofshifts.h \
    dlgmanagertools.h \
    dlgorder.h \
    ../Classes/c5user.h \
    ../Classes/c5menu.h \
    ../Classes/c5witerconf.h \
    ../Classes/c5waiterserver.h \
    ../Classes/c5ordertabledelegate.h \
    ../Classes/c5halltabledelegate.h \
    ../Classes/c5printing.h \
    ../Classes/c5printservicethread.h \
    ../Classes/QRCodeGenerator.h \
    ../../NewTax/Src/printtaxn.h \
    ../Controls/c5lineedit.h \
    ../Classes/c5printremovedservicethread.h \
    dlgprecheckoptions.h \
    dlgpreorder.h \
    dlgscreen.h \
    dlgshiftrotation.h \
    dlgsplitorder.h \
    dlgstafflist.h \
    dlgstoplistoption.h \
    dlgsystem.h \
    dlgtext.h \
    dlgtimeorder.h \
    dlgviewstoplist.h \
    qdishbutton.h \
    qdishpart2button.h \
    rc.h \
    res.rc \
    ../Controls/c5checkbox.h \
    ../Controls/c5combobox.h \
    ../Controls/c5dateedit.h \
    ../Controls/c5graphicsview.h \
    ../Classes/c5tablemodel.h \
    ../Classes/c5tableview.h \
    ../Classes/c5printpreview.h \
    ../Classes/c5widget.h \
    ../Classes/c5cache.h \
    stoplist.h \
    tablewidget.h \
    tablewidgetv1.h \
    tablewidgetv2.h \
    wguestorder.h \
    worder.h \
    wpaymentoptions.h \
    dlgcreditcardlist.h \
    dlgdishremovereason.h \
    dlglistofhall.h \
    ../Classes/c5printjson.h \
    dlgreports.h \
    dlgreportslist.h \
    dlglistofmenu.h \
    dlgguest.h \
    ../Classes/rkeyboard.h \
    dlgreceiptlanguage.h \
    dlgcl.h \
    ../Classes/c5logtoserverthread.h \
    ../Classes/c5permissions.h \
    dlgexitwithmessage.h \
    ../Classes/c5translator.h \
    c5printreceiptthread.h \
    ../Cafe5/dlgexitbyversion.h \
    ../Classes/fileversion.h \
    dlgsearchinmenu.h

FORMS += \
    ../Cafe5/calendar.ui \
    ../Classes/c5gridgilter.ui \
    ../Classes/calculator.ui \
    ../Classes/dlgpassword.ui \
    ../Classes/notificationwidget.ui \
    ../Forms/c5filtervalues.ui \
    ../Forms/change.ui \
    ../Forms/dlglistofdishcomments.ui \
    ../Forms/dlgqty.ui \
    ../Shop/selectprinters.ui \
    dishitem.ui \
    dlgaskforprecheck.ui \
    dlgcarnumber.ui \
    dlgchosesplitorderoption.ui \
        dlgface.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    dlgguests.ui \
    dlglist.ui \
    dlglistdishspecial.ui \
    dlglistofpackages.ui \
    dlglistofshifts.ui \
    dlgmanagertools.ui \
    dlgorder.ui \
    ../Classes/c5printpreview.ui \
    dlgprecheckoptions.ui \
    dlgpreorder.ui \
    dlgscreen.ui \
    dlgshiftrotation.ui \
    dlgsplitorder.ui \
    dlgstafflist.ui \
    dlgstoplistoption.ui \
    dlgsystem.ui \
    dlgtext.ui \
    dlgtimeorder.ui \
    dlgviewstoplist.ui \
    tablewidgetv1.ui \
    tablewidgetv2.ui \
    wguestorder.ui \
    worder.ui \
    wpaymentoptions.ui \
    dlgcreditcardlist.ui \
    dlgdishremovereason.ui \
    dlglistofhall.ui \
    dlgreports.ui \
    dlgreportslist.ui \
    dlglistofmenu.ui \
    dlgguest.ui \
    ../Classes/rkeyboard.ui \
    dlgreceiptlanguage.ui \
    dlgcl.ui \
    dlgexitwithmessage.ui \
    ../Cafe5/dlgexitbyversion.ui \
    dlgsearchinmenu.ui

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Cache
INCLUDEPATH += ../Controls
INCLUDEPATH += ../Forms
INCLUDEPATH += ../Service
INCLUDEPATH += ../DbData
INCLUDEPATH += ../Shop
INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include/openssl
INCLUDEPATH += C:/Projects/XLSX/src

RESOURCES += \
    ../resources/res.qrc \
    ../resources/resources.qrc \
    translator.qrc

DISTFILES += \
    cup.ico

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += -llibcrypto


win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings
