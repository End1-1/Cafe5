#-------------------------------------------------
#
# Project created by QtCreator 2018-06-21T10:59:55
#
#-------------------------------------------------

QT       += core gui network printsupport sql multimedia axcontainer websockets httpserver

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Waiter
TEMPLATE = app

RC_FILE = res.rc

ICON = cup.ico

DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"WAITER\\\"
DEFINES += WAITER
DEFINES += _CRYPT_KEY_=\\\"shop2022!!!\\\"
DEFINES += NETWORKDB
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
    ../Cafe5/c5cafecommon.cpp \
    ../Cafe5/c5double.cpp \
    ../Cafe5/c5systempreference.cpp \
    ../Cafe5/calendar.cpp \
    ../Classes/QProgressIndicator.cpp \
    ../Classes/amkbd.cpp \
    ../Classes/axreporting.cpp \
    ../Classes/c5airlog.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Classes/c5document.cpp \
    ../Classes/c5gridgilter.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5orderdriver.cpp \
    ../Classes/c5printreceiptthread.cpp \
    ../Classes/c5printrecipta4.cpp \
    ../Classes/c5random.cpp \
    ../Classes/c5replacecharacter.cpp \
    ../Classes/c5reporttemplatedriver.cpp \
    ../Classes/c5servername.cpp \
    ../Classes/c5shoporder.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5tabledata.cpp \
    ../Classes/c5tablerec.cpp \
    ../Classes/c5textdelegate.cpp \
    ../Classes/c5threadobject.cpp \
    ../Classes/c5waiterorderdoc.cpp \
    ../Classes/calculator.cpp \
    ../Classes/dataonline.cpp \
    ../Classes/dlgpassword.cpp \
    ../Classes/dlgserverconnection.cpp \
    ../Classes/idram.cpp \
    ../Classes/logwriter.cpp \
    ../Classes/notificationwidget.cpp \
    ../Classes/outputofheader.cpp \
    ../Configs/breezeconfig.cpp \
    ../Configs/cashboxconfig.cpp \
    ../Configs/configs.cpp \
    ../Controls/c5combotable.cpp \
    ../Controls/c5guicontrols.cpp \
    ../Controls/combosearchview.cpp \
    ../DbData/datadriver.cpp \
    ../DbData/dbbodystate.cpp \
    ../DbData/dbcar.cpp \
    ../DbData/dbcurrency.cpp \
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
    ../DbData/dbpartner.cpp \
    ../DbData/dbservicevalues.cpp \
    ../DbData/dbstore.cpp \
    ../DbData/dbstoredoctype.cpp \
    ../DbData/dbtables.cpp \
    ../DbData/dbunit.cpp \
    ../DbData/dbusers.cpp \
    ../Forms/armsoftexportoptions.cpp \
    ../Forms/barcode.cpp \
    ../Forms/c5changepassword.cpp \
    ../Forms/c5changepriceofgroup.cpp \
    ../Forms/c5checkdatabase.cpp \
    ../Forms/c5daterange.cpp \
    ../Forms/c5dbresetoption.cpp \
    ../Forms/c5dlgselectreporttemplate.cpp \
    ../Forms/c5filtervalues.cpp \
    ../Forms/c5passwords.cpp \
    ../Forms/c5progressdialog.cpp \
    ../Forms/c5storedocselectprinttemplate.cpp \
    ../Forms/c5toolbarwidget.cpp \
    ../Forms/change.cpp \
    ../Forms/customerinfo.cpp \
    ../Forms/dlgcashinout.cpp \
    ../Forms/dlgcashop.cpp \
    ../Forms/dlgconfigtable.cpp \
    ../Forms/dlgdataonline.cpp \
    ../Forms/dlgguestinfo.cpp \
    ../Forms/dlglist2.cpp \
    ../Forms/dlglistofdishcomments.cpp \
    ../Forms/dlgpaymentchoose.cpp \
    ../Forms/dlgprintrecipesoptions.cpp \
    ../Forms/dlgqty.cpp \
    ../Forms/dlgreservgoods.cpp \
    ../Forms/dlgselectcurrency.cpp \
    ../Forms/dlgsplashscreen.cpp \
    ../Forms/dlgstoreutils.cpp \
    ../Forms/dlgviewcashreport.cpp \
    ../Forms/httpquerydialog.cpp \
    ../Forms/mfprocessproductpriceupdate.cpp \
    ../Forms/touchdlgphonenumber.cpp \
    ../Forms/touchentertaxreceiptnumber.cpp \
    ../Forms/touchselecttaxreport.cpp \
    ../NTable/ndataprovider.cpp \
    ../NTable/nfilterdlg.cpp \
    ../NTable/ninterface.cpp \
    ../NTable/nloadingdlg.cpp \
    ../NTable/nsearchdlg.cpp \
    ../NTable/ntablemodel.cpp \
    ../Printing/printreceipt.cpp \
    ../Printing/printreceiptgroup.cpp \
    ../Printing/selectprinters.cpp \
    ../Service5Working/utils/configini.cpp \
    ../TableRecord/aheader.cpp \
    ../TableRecord/bclientdebts.cpp \
    ../TableRecord/bhistory.cpp \
    ../TableRecord/c5dbrecord.cpp \
    ../TableRecord/cpartners.cpp \
    ../TableRecord/ecash.cpp \
    ../TableRecord/odraftsale.cpp \
    ../TableRecord/odraftsalebody.cpp \
    ../TableRecord/ogoods.cpp \
    ../TableRecord/oheader.cpp \
    ../TableRecord/otax.cpp \
    dishitem.cpp \
    dishitembutton.cpp \
    dlgaskforprecheck.cpp \
    dlgcarnumber.cpp \
    dlgcashbuttonopions.cpp \
    dlgeditphone.cpp \
    dlgguests.cpp \
    dlglist.cpp \
    dlglistdishspecial.cpp \
    dlglistofpackages.cpp \
    dlgmanagertools.cpp \
    dlgmenuset.cpp \
    dlgprecheckoptions.cpp \
    dlgpreorderw.cpp \
    dlgscreen.cpp \
    dlgsearchinmenu.cpp \
    dlgshiftrotation.cpp \
    dlgsplitorder.cpp \
    dlgstafflist.cpp \
    dlgstoplistoption.cpp \
    dlgtext.cpp \
    dlgtimeorder.cpp \
    dlgviewstoplist.cpp \
        main.cpp \
        dlgface.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5message.cpp \
    dlgorder.cpp \
    ../Classes/c5user.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5witerconf.cpp \
    ../Classes/c5waiterserver.cpp \
    ../Classes/c5ordertabledelegate.cpp \
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
    waitertablewidget.cpp \
    wguestorder.cpp \
    worderw.cpp \
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
    ../Classes/fileversion.cpp

HEADERS += \
    ../Cafe5/c5cafecommon.h \
    ../Cafe5/c5double.h \
    ../Cafe5/c5systempreference.h \
    ../Cafe5/calendar.h \
    ../Classes/QProgressIndicator.h \
    ../Classes/amkbd.h \
    ../Classes/axreporting.h \
    ../Classes/barcode5.h \
    ../Classes/c5airlog.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5datedelegate.h \
    ../Classes/c5document.h \
    ../Classes/c5gridgilter.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5orderdriver.h \
    ../Classes/c5printreceiptthread.h \
    ../Classes/c5printrecipta4.h \
    ../Classes/c5random.h \
    ../Classes/c5replacecharacter.h \
    ../Classes/c5reporttemplatedriver.h \
    ../Classes/c5servername.h \
    ../Classes/c5shoporder.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5tabledata.h \
    ../Classes/c5tablerec.h \
    ../Classes/c5textdelegate.h \
    ../Classes/c5threadobject.h \
    ../Classes/c5waiterorderdoc.h \
    ../Classes/calculator.h \
    ../Classes/chatmessage.h \
    ../Classes/dataonline.h \
    ../Classes/dlgpassword.h \
    ../Classes/dlgserverconnection.h \
    ../Classes/goodsreserve.h \
    ../Classes/idram.h \
    ../Classes/logwriter.h \
    ../Classes/notificationwidget.h \
    ../Classes/outputofheader.h \
    ../Configs/breezeconfig.h \
    ../Configs/cashboxconfig.h \
    ../Configs/configs.h \
    ../Controls/c5combotable.h \
    ../Controls/c5guicontrols.h \
    ../Controls/combosearchview.h \
    ../DbData/datadriver.h \
    ../DbData/dbbodystate.h \
    ../DbData/dbcar.h \
    ../DbData/dbcurrency.h \
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
    ../DbData/dbpartner.h \
    ../DbData/dbservicevalues.h \
    ../DbData/dbstore.h \
    ../DbData/dbstoredoctype.h \
    ../DbData/dbtables.h \
    ../DbData/dbunit.h \
    ../DbData/dbusers.h \
    ../Forms/armsoftexportoptions.h \
    ../Forms/barcode.h \
    ../Forms/c5changepassword.h \
    ../Forms/c5changepriceofgroup.h \
    ../Forms/c5checkdatabase.h \
    ../Forms/c5daterange.h \
    ../Forms/c5dbresetoption.h \
    ../Forms/c5dlgselectreporttemplate.h \
    ../Forms/c5filtervalues.h \
    ../Forms/c5passwords.h \
    ../Forms/c5progressdialog.h \
    ../Forms/c5storedocselectprinttemplate.h \
    ../Forms/c5toolbarwidget.h \
    ../Forms/change.h \
    ../Forms/customerinfo.h \
    ../Forms/dlgcashinout.h \
    ../Forms/dlgcashop.h \
    ../Forms/dlgconfigtable.h \
    ../Forms/dlgdataonline.h \
    ../Forms/dlgguestinfo.h \
    ../Forms/dlglist2.h \
    ../Forms/dlglistofdishcomments.h \
    ../Forms/dlgpaymentchoose.h \
    ../Forms/dlgprintrecipesoptions.h \
    ../Forms/dlgqty.h \
    ../Forms/dlgreservgoods.h \
    ../Forms/dlgselectcurrency.h \
    ../Forms/dlgsplashscreen.h \
    ../Forms/dlgstoreutils.h \
    ../Forms/dlgviewcashreport.h \
    ../Forms/httpquerydialog.h \
    ../Forms/mfprocessproductpriceupdate.h \
    ../Forms/touchdlgphonenumber.h \
    ../Forms/touchentertaxreceiptnumber.h \
    ../Forms/touchselecttaxreport.h \
    ../NTable/ndataprovider.h \
    ../NTable/nfilterdlg.h \
    ../NTable/ninterface.h \
    ../NTable/nloadingdlg.h \
    ../NTable/nsearchdlg.h \
    ../NTable/ntablemodel.h \
    ../Printing/printreceipt.h \
    ../Printing/printreceiptgroup.h \
    ../Printing/selectprinters.h \
    ../Service5Working/utils/configini.h \
    ../Service5Working/utils/servicecommands.h \
    ../TableRecord/aheader.h \
    ../TableRecord/bclientdebts.h \
    ../TableRecord/bhistory.h \
    ../TableRecord/c5dbrecord.h \
    ../TableRecord/cpartners.h \
    ../TableRecord/ecash.h \
    ../TableRecord/odraftsale.h \
    ../TableRecord/odraftsalebody.h \
    ../TableRecord/ogoods.h \
    ../TableRecord/oheader.h \
    ../TableRecord/otax.h \
    dishitem.h \
    dishitembutton.h \
    dlgaskforprecheck.h \
    dlgcarnumber.h \
    dlgcashbuttonopions.h \
    dlgeditphone.h \
        dlgface.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5message.h \
    dlgguests.h \
    dlglist.h \
    dlglistdishspecial.h \
    dlglistofpackages.h \
    dlgmanagertools.h \
    dlgmenuset.h \
    dlgorder.h \
    ../Classes/c5user.h \
    ../Classes/c5menu.h \
    ../Classes/c5witerconf.h \
    ../Classes/c5waiterserver.h \
    ../Classes/c5ordertabledelegate.h \
    ../Classes/c5printing.h \
    ../Classes/c5printservicethread.h \
    ../Classes/QRCodeGenerator.h \
    ../../NewTax/Src/printtaxn.h \
    ../Controls/c5lineedit.h \
    ../Classes/c5printremovedservicethread.h \
    dlgprecheckoptions.h \
    dlgpreorderw.h \
    dlgscreen.h \
    dlgsearchinmenu.h \
    dlgshiftrotation.h \
    dlgsplitorder.h \
    dlgstafflist.h \
    dlgstoplistoption.h \
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
    tablewidget.h \
    tablewidgetv1.h \
    waitertablewidget.h \
    wguestorder.h \
    worderw.h \
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
    ../Cafe5/dlgexitbyversion.h \
    ../Classes/fileversion.h

FORMS += \
    ../Cafe5/calendar.ui \
    ../Classes/c5gridgilter.ui \
    ../Classes/calculator.ui \
    ../Classes/dlgpassword.ui \
    ../Classes/dlgserverconnection.ui \
    ../Classes/notificationwidget.ui \
    ../Controls/combosearchview.ui \
    ../Forms/armsoftexportoptions.ui \
    ../Forms/c5changepassword.ui \
    ../Forms/c5changepriceofgroup.ui \
    ../Forms/c5checkdatabase.ui \
    ../Forms/c5daterange.ui \
    ../Forms/c5dbresetoption.ui \
    ../Forms/c5dlgselectreporttemplate.ui \
    ../Forms/c5filtervalues.ui \
    ../Forms/c5passwords.ui \
    ../Forms/c5progressdialog.ui \
    ../Forms/c5storedocselectprinttemplate.ui \
    ../Forms/c5toolbarwidget.ui \
    ../Forms/change.ui \
    ../Forms/customerinfo.ui \
    ../Forms/dlgcashinout.ui \
    ../Forms/dlgcashop.ui \
    ../Forms/dlgconfigtable.ui \
    ../Forms/dlgdataonline.ui \
    ../Forms/dlgguestinfo.ui \
    ../Forms/dlglist2.ui \
    ../Forms/dlglistofdishcomments.ui \
    ../Forms/dlgpaymentchoose.ui \
    ../Forms/dlgprintrecipesoptions.ui \
    ../Forms/dlgqty.ui \
    ../Forms/dlgreservgoods.ui \
    ../Forms/dlgselectcurrency.ui \
    ../Forms/dlgsplashscreen.ui \
    ../Forms/dlgstoreutils.ui \
    ../Forms/dlgviewcashreport.ui \
    ../Forms/httpquerydialog.ui \
    ../Forms/mfprocessproductpriceupdate.ui \
    ../Forms/touchdlgphonenumber.ui \
    ../Forms/touchentertaxreceiptnumber.ui \
    ../Forms/touchselecttaxreport.ui \
    ../NTable/nfilterdlg.ui \
    ../NTable/nloadingdlg.ui \
    ../NTable/nsearchdlg.ui \
    ../Printing/preorders.ui \
    ../Printing/selectprinters.ui \
    dishitem.ui \
    dlgaskforprecheck.ui \
    dlgcarnumber.ui \
    dlgcashbuttonopions.ui \
    dlgeditphone.ui \
        dlgface.ui \
    ../Cafe5/c5message.ui \
    dlgguests.ui \
    dlglist.ui \
    dlglistdishspecial.ui \
    dlglistofpackages.ui \
    dlgmanagertools.ui \
    dlgmenuset.ui \
    dlgorder.ui \
    ../Classes/c5printpreview.ui \
    dlgprecheckoptions.ui \
    dlgpreorderw.ui \
    dlgscreen.ui \
    dlgsearchinmenu.ui \
    dlgshiftrotation.ui \
    dlgsplitorder.ui \
    dlgstafflist.ui \
    dlgstoplistoption.ui \
    dlgtext.ui \
    dlgtimeorder.ui \
    dlgviewstoplist.ui \
    tablewidgetv1.ui \
    wguestorder.ui \
    worderw.ui \
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
    ../Cafe5/dlgexitbyversion.ui

    INCLUDEPATH += ../Cafe5
    INCLUDEPATH += ../Classes
    INCLUDEPATH += ../Cache
    INCLUDEPATH += ../Controls
    INCLUDEPATH += ../Editors
    INCLUDEPATH += ../Forms
    INCLUDEPATH += ../Service
    INCLUDEPATH += ../Configs
    INCLUDEPATH += ../Printing
    INCLUDEPATH += ../NTable
    INCLUDEPATH += ../Service5Working/raw
    INCLUDEPATH += ../Service5Working/socket
    INCLUDEPATH += ../DbData
    INCLUDEPATH += ../Configs
    INCLUDEPATH += ../TableRecord
    INCLUDEPATH += ../Controls
    INCLUDEPATH += ../NTable
    INCLUDEPATH += C:/Projects/NewTax/Src
    INCLUDEPATH += C:/Soft/OpenSSLWin64/include
    INCLUDEPATH += C:/Soft/OpenSSLWin64/include/openssl

RESOURCES += \
    ../resources/res.qrc \
    translator.qrc

DISTFILES += \
    cup.ico

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += -llibcrypto

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings
