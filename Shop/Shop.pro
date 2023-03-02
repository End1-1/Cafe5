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
    ../Cafe5/c5replication.cpp \
    ../Cafe5/c5serverhandler.cpp \
    ../Cafe5/c5systempreference.cpp \
    ../Cafe5/calendar.cpp \
    ../Cafe5/dlgexitbyversion.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5crypt.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Classes/c5filelogwriter.cpp \
    ../Classes/c5gridgilter.cpp \
    ../Classes/c5halltabledelegate.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5logtoserverthread.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5orderdriver.cpp \
    ../Classes/c5ordertabledelegate.cpp \
    ../Classes/c5permissions.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printjson.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5printrecipta4.cpp \
    ../Classes/c5printremovedservicethread.cpp \
    ../Classes/c5printservicethread.cpp \
    ../Classes/c5random.cpp \
    ../Classes/c5shoporder.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5tablerec.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5textdelegate.cpp \
    ../Classes/c5threadobject.cpp \
    ../Classes/c5translator.cpp \
    ../Classes/c5user.cpp \
    ../Classes/c5waiterorderdoc.cpp \
    ../Classes/c5widget.cpp \
    ../Classes/c5witerconf.cpp \
    ../Classes/calculator.cpp \
    ../Classes/checkforupdatethread.cpp \
    ../Classes/config.cpp \
    ../Classes/dataonline.cpp \
    ../Classes/doubledatabase.cpp \
    ../Classes/fileversion.cpp \
    ../Classes/notificationwidget.cpp \
    ../Classes/proxytablewidgetdatabase.cpp \
    ../Classes/rkeyboard.cpp \
    ../Classes/socketconnection.cpp \
    ../Classes/threadsendmessage.cpp \
    ../Controls/c5cleartablewidget.cpp \
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
    ../Forms/c5printtaxanywhere.cpp \
    ../Forms/dlgdataonline.cpp \
    ../Forms/dlgqrcode.cpp \
    ../Forms/dlgreservgoods.cpp \
    ../Forms/dlgserversettings.cpp \
    ../Service5/raw/rawmessage.cpp \
    ../Service5/socket/sslsocket.cpp \
    c5tempsale.cpp \
    cashcollection.cpp \
    dlgdate.cpp \
    dlggetidname.cpp \
    dlggiftcardsale.cpp \
    dlggoodslist.cpp \
    dlgpin.cpp \
    dlgpreorder.cpp \
    dlgpreorderitem.cpp \
    dlgreturnitem.cpp \
    dlgsplashscreen.cpp \
    goodsreturnreason.cpp \
    imageloader.cpp \
    loghistory.cpp \
        main.cpp \
    preorders.cpp \
    printreceipt.cpp \
    printreceiptgroup.cpp \
    replicadialog.cpp \
    sales.cpp \
    searchitems.cpp \
    selectprinters.cpp \
    selectstaff.cpp \
    settingsselection.cpp \
    storeinput.cpp \
    taxprint.cpp \
    threadcheckmessage.cpp \
    threadreadmessage.cpp \
    threadworker.cpp \
    userphoto.cpp \
    viewinputitem.cpp \
    vieworder.cpp \
    wcustomerdisplay.cpp \
        working.cpp \
    worder.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5sockethandler.cpp \
    ../Controls/c5lineedit.cpp \
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
    ../Cafe5/c5replication.h \
    ../Cafe5/c5serverhandler.h \
    ../Cafe5/c5systempreference.h \
    ../Cafe5/calendar.h \
    ../Cafe5/dlgexitbyversion.h \
    ../Classes/QRCodeGenerator.h \
    ../Classes/barcode5.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5crypt.h \
    ../Classes/c5datedelegate.h \
    ../Classes/c5filelogwriter.h \
    ../Classes/c5gridgilter.h \
    ../Classes/c5halltabledelegate.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5logtoserverthread.h \
    ../Classes/c5menu.h \
    ../Classes/c5orderdriver.h \
    ../Classes/c5ordertabledelegate.h \
    ../Classes/c5permissions.h \
    ../Classes/c5printing.h \
    ../Classes/c5printjson.h \
    ../Classes/c5printpreview.h \
    ../Classes/c5printrecipta4.h \
    ../Classes/c5printremovedservicethread.h \
    ../Classes/c5printservicethread.h \
    ../Classes/c5random.h \
    ../Classes/c5shoporder.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5tablemodel.h \
    ../Classes/c5tablerec.h \
    ../Classes/c5tableview.h \
    ../Classes/c5textdelegate.h \
    ../Classes/c5threadobject.h \
    ../Classes/c5translator.h \
    ../Classes/c5user.h \
    ../Classes/c5waiterorderdoc.h \
    ../Classes/c5widget.h \
    ../Classes/c5witerconf.h \
    ../Classes/calculator.h \
    ../Classes/chatmessage.h \
    ../Classes/checkforupdatethread.h \
    ../Classes/config.h \
    ../Classes/dataonline.h \
    ../Classes/doubledatabase.h \
    ../Classes/fileversion.h \
    ../Classes/goodsreserve.h \
    ../Classes/notificationwidget.h \
    ../Classes/proxytablewidgetdatabase.h \
    ../Classes/rkeyboard.h \
    ../Classes/socketconnection.h \
    ../Classes/threadsendmessage.h \
    ../Controls/c5cleartablewidget.h \
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
    ../Forms/c5printtaxanywhere.h \
    ../Forms/dlgdataonline.h \
    ../Forms/dlgqrcode.h \
    ../Forms/dlgreservgoods.h \
    ../Forms/dlgserversettings.h \
    ../Service5/raw/rawmessage.h \
    ../Service5/socket/sslsocket.h \
    c5tempsale.h \
    cashcollection.h \
    dlgdate.h \
    dlggetidname.h \
    dlggiftcardsale.h \
    dlggoodslist.h \
    dlgpin.h \
    dlgpreorder.h \
    dlgpreorderitem.h \
    dlgreturnitem.h \
    dlgsplashscreen.h \
    goodsreturnreason.h \
    imageloader.h \
    loghistory.h \
    preorders.h \
    printreceipt.h \
    printreceiptgroup.h \
    replicadialog.h \
    sales.h \
    searchitems.h \
    selectprinters.h \
    selectstaff.h \
    settingsselection.h \
    storeinput.h \
    taxprint.h \
    threadcheckmessage.h \
    threadreadmessage.h \
    threadworker.h \
    userphoto.h \
    viewinputitem.h \
    vieworder.h \
    wcustomerdisplay.h \
        working.h \
    worder.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5sockethandler.h \
    ../Cafe5/c5socketmessage.h \
    ../Controls/c5lineedit.h \
    ../Controls/c5combobox.h \
    ../Controls/c5dateedit.h \
    ../Classes/c5cache.h \
    ../../NewTax/Src/printtaxn.h \
    dqty.h \
    ../Controls/c5checkbox.h

FORMS += \
    ../Cafe5/calendar.ui \
    ../Cafe5/dlgexitbyversion.ui \
    ../Classes/c5gridgilter.ui \
    ../Classes/c5printpreview.ui \
    ../Classes/calculator.ui \
    ../Classes/notificationwidget.ui \
    ../Classes/rkeyboard.ui \
    ../Controls/c5tablewithtotal.ui \
    ../Forms/c5printtaxanywhere.ui \
    ../Forms/dlgdataonline.ui \
    ../Forms/dlgqrcode.ui \
    ../Forms/dlgreservgoods.ui \
    ../Forms/dlgserversettings.ui \
    c5tempsale.ui \
    cashcollection.ui \
    dlgdate.ui \
    dlggetidname.ui \
    dlggiftcardsale.ui \
    dlggoodslist.ui \
    dlgpin.ui \
    dlgpreorder.ui \
    dlgpreorderitem.ui \
    dlgreturnitem.ui \
    dlgsplashscreen.ui \
    goodsreturnreason.ui \
    loghistory.ui \
    preorders.ui \
    replicadialog.ui \
    sales.ui \
    searchitems.ui \
    selectprinters.ui \
    selectstaff.ui \
    settingsselection.ui \
    storeinput.ui \
    taxprint.ui \
    userphoto.ui \
    viewinputitem.ui \
    vieworder.ui \
    wcustomerdisplay.ui \
        working.ui \
    worder.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    dqty.ui

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"BreezeDevs\\\"
DEFINES += _APPLICATION_=\\\"Shop\\\"
DEFINES += _MODULE_=\\\"Shop\\\"
DEFINES += SHOP

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Cache
INCLUDEPATH += ../Controls
INCLUDEPATH += ../DbData
INCLUDEPATH += ../Forms
INCLUDEPATH += ../Service
INCLUDEPATH += ../Service5
INCLUDEPATH += ../Service5/raw
INCLUDEPATH += ../Service5/socket
INCLUDEPATH += ../RESOURCES
INCLUDEPATH += ../../XLSX/Src
INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include/openssl

ICON = bag.ico

RESOURCES += \
    ../resources/res.qrc \
    local.qrc

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += -llibcrypto

DEFINES += _CRYPT_KEY_=\\\"shop2022!!!\\\"
DEFINES += WAITER

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

