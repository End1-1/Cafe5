#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T11:27:26
#
#-------------------------------------------------

QT       += core gui printsupport network sql multimedia websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Shop.net
TEMPLATE = app

win32 {
    RC_FILE = res.rc
}

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
    ../Cafe5/c5systempreference.cpp \
    ../Cafe5/calendar.cpp \
    ../Cafe5/dlgexitbyversion.cpp \
    ../Classes/QProgressIndicator.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../Classes/appwebsocket.cpp \
    ../Classes/c5airlog.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5logtoserverthread.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5permissions.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printjson.cpp \
    ../Classes/c5printrecipta4.cpp \
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
    ../DbData/dbdata.cpp \
    ../DbData/dbgoods.cpp \
    ../DbData/dbgoodsgroup.cpp \
    ../DbData/dbstore.cpp \
    ../DbData/dbstoredoctype.cpp \
    ../DbData/dbunit.cpp \
    ../Forms/c5connectiondialog.cpp \
    ../Forms/dlgpaymentchoose.cpp \
    ../Forms/dlgreservgoods.cpp \
    ../Forms/dlgsplashscreen.cpp \
    ../NTable/ndataprovider.cpp \
    ../NTable/ninterface.cpp \
    ../NTable/nloadingdlg.cpp \
    ../Printing/printreceipt.cpp \
    ../Printing/printreceiptgroup.cpp \
    ../Printing/selectprinters.cpp \
    ../StructModel/c5structtableview.cpp \
    ../TableRecord/aheader.cpp \
    ../TableRecord/bclientdebts.cpp \
    ../TableRecord/bhistory.cpp \
    ../TableRecord/c5dbrecord.cpp \
    ../TableRecord/cpartners.cpp \
    ../TableRecord/ecash.cpp \
    ../TableRecord/odraftsale.cpp \
    ../TableRecord/ogoods.cpp \
    ../TableRecord/oheader.cpp \
    ../TableRecord/otax.cpp \
    c5shopdialog.cpp \
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
    dlgshowcolumns.cpp \
    goodsreturnreason.cpp \
        main.cpp \
    sales.cpp \
    searchitems.cpp \
    selectstaff.cpp \
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
    ../Cafe5/c5systempreference.h \
    ../Cafe5/calendar.h \
    ../Cafe5/dlgexitbyversion.h \
    ../Classes/QProgressIndicator.h \
    ../Classes/QRCodeGenerator.h \
    ../Classes/appwebsocket.h \
    ../Classes/barcode5.h \
    ../Classes/c5airlog.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5datedelegate.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5logtoserverthread.h \
    ../Classes/c5menu.h \
    ../Classes/c5permissions.h \
    ../Classes/c5printing.h \
    ../Classes/c5printjson.h \
    ../Classes/c5printrecipta4.h \
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
    ../DbData/dbdata.h \
    ../DbData/dbgoods.h \
    ../DbData/dbgoodsgroup.h \
    ../DbData/dbhalls.h \
    ../DbData/dbstore.h \
    ../DbData/dbstoredoctype.h \
    ../DbData/dbunit.h \
    ../DbData/dbusers.h \
    ../Forms/c5connectiondialog.h \
    ../Forms/dlgpaymentchoose.h \
    ../Forms/dlgreservgoods.h \
    ../Forms/dlgsplashscreen.h \
    ../NTable/ndataprovider.h \
    ../NTable/ninterface.h \
    ../NTable/nloadingdlg.h \
    ../Printing/printreceipt.h \
    ../Printing/printreceiptgroup.h \
    ../Printing/selectprinters.h \
    ../Service5/worker/StructStorage.h \
    ../Service5/worker/c5jsonparser.h \
    ../Service5/worker/c5structtraits.h \
    ../Service5/worker/struct_goods_item.h \
    ../Service5/worker/struct_partner.h \
    ../Service5/worker/struct_storage_item.h \
    ../StructModel/c5structmodel.h \
    ../StructModel/c5structtableview.h \
    ../TableRecord/aheader.h \
    ../TableRecord/bclientdebts.h \
    ../TableRecord/bhistory.h \
    ../TableRecord/c5dbrecord.h \
    ../TableRecord/cpartners.h \
    ../TableRecord/ecash.h \
    ../TableRecord/odraftsale.h \
    ../TableRecord/ogoods.h \
    ../TableRecord/oheader.h \
    ../TableRecord/otax.h \
    c5shopdialog.h \
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
    dlgshowcolumns.h \
    goodscols.h \
    goodsreturnreason.h \
    rc.h \
    sales.h \
    searchitems.h \
    selectstaff.h \
    storeinput.h \
    threadworker.h \
    userphoto.h \
    version.h \
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
    ../Classes/calculator.ui \
    ../Classes/notificationwidget.ui \
    ../Controls/c5tablewithtotal.ui \
    ../Forms/c5connectiondialog.ui \
    ../Forms/dlgpaymentchoose.ui \
    ../Forms/dlgreservgoods.ui \
    ../Forms/dlgsplashscreen.ui \
    ../NTable/nloadingdlg.ui \
    ../Printing/selectprinters.ui \
    ../StructModel/c5structtableview.ui \
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
    dlgshowcolumns.ui \
    goodsreturnreason.ui \
    sales.ui \
    searchitems.ui \
    selectstaff.ui \
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
INCLUDEPATH += ../Service5/worker
INCLUDEPATH += ../StructModel
INCLUDEPATH += ../Printing
INCLUDEPATH += ../RESOURCES
INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += c:/Soft/OpenSSLWin64/include
INCLUDEPATH += c:/Soft/OpenSSLWin64/include/openssl

ICON = bag.ico

RESOURCES += \
    ../resources/res.qrc \
    local.qrc

include(C:/projects/QXlsx/QXlsx.pri)

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += -llibcrypto

DEFINES += _CRYPT_KEY_=\\\"shop2022!!!\\\"
DEFINES += BORDERLESSDIALOGS

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

DISTFILES += \
    increase_version.ps1
win32 {
    version_inc.target = version_inc
    version_inc.commands = powershell -NoProfile -ExecutionPolicy Bypass -File $$shell_path($$PWD/increase_version.ps1)
    QMAKE_EXTRA_TARGETS += version_inc
    PRE_TARGETDEPS += version_inc
}


win32 {
    res_rc.depends += $$PWD/version.h
}
