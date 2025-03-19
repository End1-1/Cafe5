#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T11:27:26
#
#-------------------------------------------------

QT       += core gui printsupport network sql multimedia websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Shop.net
TEMPLATE = app

RC_FILE = res.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += NETWORKDB
#DEFINES += BF10

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
# disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../Cafe5/c5cafecommon.cpp \
    ../Cafe5/c5double.cpp \
    ../Cafe5/c5sockethandler.cpp \
    ../Cafe5/c5systempreference.cpp \
    ../Cafe5/calendar.cpp \
    ../Cafe5/dlgexitbyversion.cpp \
    ../Classes/QProgressIndicator.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../Classes/c5airlog.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Classes/c5document.cpp \
    ../Classes/c5gridgilter.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5logtoserverthread.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5permissions.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printjson.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5printreceiptthread.cpp \
    ../Classes/c5printrecipta4.cpp \
    ../Classes/c5printremovedservicethread.cpp \
    ../Classes/c5random.cpp \
    ../Classes/c5replacecharacter.cpp \
    ../Classes/c5reporttemplatedriver.cpp \
    ../Classes/c5servername.cpp \
    ../Classes/c5shoporder.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5tablerec.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5textdelegate.cpp \
    ../Classes/c5threadobject.cpp \
    ../Classes/c5translator.cpp \
    ../Classes/c5user.cpp \
    ../Classes/c5widget.cpp \
    ../Classes/c5witerconf.cpp \
    ../Classes/calculator.cpp \
    ../Classes/dataonline.cpp \
    ../Classes/dlgpassword.cpp \
    ../Classes/fileversion.cpp \
    ../Classes/idram.cpp \
    ../Classes/logwriter.cpp \
    ../Classes/notificationwidget.cpp \
    ../Classes/outputofheader.cpp \
    ../Classes/proxytablewidgetdatabase.cpp \
    ../Configs/breezeconfig.cpp \
    ../Configs/cashboxconfig.cpp \
    ../Configs/configs.cpp \
    ../Controls/c5cleartablewidget.cpp \
    ../Controls/c5graphicsview.cpp \
    ../Controls/c5guicontrols.cpp \
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
    ../Forms/dlgdataonline.cpp \
    ../Forms/dlgpaymentchoose.cpp \
    ../Forms/dlgreservgoods.cpp \
    ../Forms/dlgsplashscreen.cpp \
    ../NTable/ndataprovider.cpp \
    ../NTable/ninterface.cpp \
    ../NTable/nloadingdlg.cpp \
    ../Printing/printreceipt.cpp \
    ../Printing/printreceiptgroup.cpp \
    ../Printing/selectprinters.cpp \
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
    c5tempsale.cpp \
    cashcollection.cpp \
    dlgcashout.cpp \
    dlgdate.cpp \
    dlggetidname.cpp \
    dlggetvalue.cpp \
    dlggiftcardsale.cpp \
    dlggoodslist.cpp \
    dlgmovement.cpp \
    dlgpin.cpp \
    dlgregistercard.cpp \
    dlgreturnitem.cpp \
    dlgsearchpartner.cpp \
    dlgshowcolumns.cpp \
    goodsreturnreason.cpp \
    imageloader.cpp \
        main.cpp \
    sales.cpp \
    searchitems.cpp \
    selectstaff.cpp \
    settingsselection.cpp \
    storeinput.cpp \
    threadworker.cpp \
    userphoto.cpp \
    viewinputitem.cpp \
    vieworder.cpp \
    wcustomerdisplay.cpp \
        working.cpp \
    worder.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Controls/c5lineedit.cpp \
    ../Controls/c5combobox.cpp \
    ../Controls/c5dateedit.cpp \
    ../Classes/c5cache.cpp \
    ../../NewTax/Src/printtaxn.cpp \
    dqty.cpp \
    ../Controls/c5checkbox.cpp

HEADERS += \
    ../Cafe5/c5cafecommon.h \
    ../Cafe5/c5double.h \
    ../Cafe5/c5sockethandler.h \
    ../Cafe5/c5systempreference.h \
    ../Cafe5/calendar.h \
    ../Cafe5/dlgexitbyversion.h \
    ../Classes/QProgressIndicator.h \
    ../Classes/QRCodeGenerator.h \
    ../Classes/barcode5.h \
    ../Classes/c5airlog.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5datedelegate.h \
    ../Classes/c5document.h \
    ../Classes/c5gridgilter.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5logtoserverthread.h \
    ../Classes/c5menu.h \
    ../Classes/c5permissions.h \
    ../Classes/c5printing.h \
    ../Classes/c5printjson.h \
    ../Classes/c5printpreview.h \
    ../Classes/c5printreceiptthread.h \
    ../Classes/c5printrecipta4.h \
    ../Classes/c5printremovedservicethread.h \
    ../Classes/c5random.h \
    ../Classes/c5replacecharacter.h \
    ../Classes/c5reporttemplatedriver.h \
    ../Classes/c5servername.h \
    ../Classes/c5shoporder.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5tablemodel.h \
    ../Classes/c5tablerec.h \
    ../Classes/c5tableview.h \
    ../Classes/c5textdelegate.h \
    ../Classes/c5threadobject.h \
    ../Classes/c5translator.h \
    ../Classes/c5user.h \
    ../Classes/c5widget.h \
    ../Classes/c5witerconf.h \
    ../Classes/calculator.h \
    ../Classes/chatmessage.h \
    ../Classes/dataonline.h \
    ../Classes/dlgpassword.h \
    ../Classes/fileversion.h \
    ../Classes/goodsreserve.h \
    ../Classes/idram.h \
    ../Classes/logwriter.h \
    ../Classes/notificationwidget.h \
    ../Classes/outputofheader.h \
    ../Classes/proxytablewidgetdatabase.h \
    ../Configs/breezeconfig.h \
    ../Configs/cashboxconfig.h \
    ../Configs/configs.h \
    ../Controls/c5cleartablewidget.h \
    ../Controls/c5graphicsview.h \
    ../Controls/c5guicontrols.h \
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
    ../Forms/dlgdataonline.h \
    ../Forms/dlgpaymentchoose.h \
    ../Forms/dlgreservgoods.h \
    ../Forms/dlgsplashscreen.h \
    ../NTable/ndataprovider.h \
    ../NTable/ninterface.h \
    ../NTable/nloadingdlg.h \
    ../Printing/printreceipt.h \
    ../Printing/printreceiptgroup.h \
    ../Printing/selectprinters.h \
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
    c5tempsale.h \
    cashcollection.h \
    dlgcashout.h \
    dlgdate.h \
    dlggetidname.h \
    dlggetvalue.h \
    dlggiftcardsale.h \
    dlggoodslist.h \
    dlgmovement.h \
    dlgpin.h \
    dlgregistercard.h \
    dlgreturnitem.h \
    dlgsearchpartner.h \
    dlgshowcolumns.h \
    goodsreturnreason.h \
    imageloader.h \
    sales.h \
    searchitems.h \
    selectstaff.h \
    settingsselection.h \
    storeinput.h \
    threadworker.h \
    userphoto.h \
    viewinputitem.h \
    vieworder.h \
    wcustomerdisplay.h \
        working.h \
    worder.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5dialog.h \
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
    ../Classes/dlgpassword.ui \
    ../Classes/notificationwidget.ui \
    ../Controls/c5tablewithtotal.ui \
    ../Forms/dlgdataonline.ui \
    ../Forms/dlgpaymentchoose.ui \
    ../Forms/dlgreservgoods.ui \
    ../Forms/dlgsplashscreen.ui \
    ../NTable/nloadingdlg.ui \
    ../Printing/selectprinters.ui \
    c5tempsale.ui \
    cashcollection.ui \
    dlgcashout.ui \
    dlgdate.ui \
    dlggetidname.ui \
    dlggetvalue.ui \
    dlggiftcardsale.ui \
    dlggoodslist.ui \
    dlgmovement.ui \
    dlgpin.ui \
    dlgregistercard.ui \
    dlgreturnitem.ui \
    dlgsearchpartner.ui \
    dlgshowcolumns.ui \
    goodsreturnreason.ui \
    sales.ui \
    searchitems.ui \
    selectstaff.ui \
    settingsselection.ui \
    storeinput.ui \
    userphoto.ui \
    viewinputitem.ui \
    vieworder.ui \
    wcustomerdisplay.ui \
        working.ui \
    worder.ui \
    ../Cafe5/c5message.ui \
    dqty.ui

DEFINES += _DBDRIVER_=\\\"QMARIADB\\\"
DEFINES += _ORGANIZATION_=\\\"BreezeDevs\\\"
DEFINES += _APPLICATION_=\\\"Shop\\\"
DEFINES += _MODULE_=\\\"Shop\\\"
DEFINES += SHOP

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../TableRecord
INCLUDEPATH += ../NTable
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Cache
INCLUDEPATH += ../Controls
INCLUDEPATH += ../DbData
INCLUDEPATH += ../Forms
INCLUDEPATH += ../Configs
INCLUDEPATH += ../Service
INCLUDEPATH += ../Printing
INCLUDEPATH += ../RESOURCES
INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += c:/Soft/OpenSSLWin64/include
INCLUDEPATH += c:/Soft/OpenSSLWin64/include/openssl

ICON = bag.ico

RESOURCES += \
    ../resources/res.qrc \
    local.qrc

include(C:/projects/QXlsx/QXlsx/QXlsx.pri)

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += -llibcrypto

DEFINES += _CRYPT_KEY_=\\\"shop2022!!!\\\"
DEFINES += WAITER

#win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
#win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
#win32: QMAKE_CFLAGS -= -Zc:strictStrings
#win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

#win32-g++ {
#   QMAKE_CXXFLAGS += -Werror
#}
##win32-msvc*{
 #  QMAKE_CXXFLAGS += /WX
#}
