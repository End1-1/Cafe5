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
    ../Cafe5/c5license.cpp \
    ../Cafe5/c5licensedlg.cpp \
    ../Cafe5/c5logsystem.cpp \
    ../Cafe5/c5replication.cpp \
    ../Cafe5/calendar.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5crypt.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Classes/c5filelogwriter.cpp \
    ../Classes/c5grid.cpp \
    ../Classes/c5gridgilter.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5orderdriver.cpp \
    ../Classes/c5random.cpp \
    ../Classes/c5shoporder.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5tablerec.cpp \
    ../Classes/c5textdelegate.cpp \
    ../Classes/c5threadobject.cpp \
    ../Classes/c5userauth.cpp \
    ../Classes/calculator.cpp \
    ../Classes/checkforupdatethread.cpp \
    ../Classes/notificationwidget.cpp \
    ../Classes/proxytablewidgetdatabase.cpp \
    ../Controls/c5guicontrols.cpp \
    ../Controls/c5lineeditwithselector.cpp \
    ../DbData/datadriver.cpp \
    ../DbData/dbdata.cpp \
    ../DbData/dbhalls.cpp \
    ../DbData/dbtables.cpp \
    ../DbData/dbusers.cpp \
    ../Editors/c5cashname.cpp \
    ../Editors/c5dishwidget.cpp \
    ../Editors/c5editor.cpp \
    ../Editors/ce5createtablesforhall.cpp \
    ../Editors/ce5creditcard.cpp \
    ../Editors/ce5databases.cpp \
    ../Editors/ce5discountcard.cpp \
    ../Editors/ce5dishcomment.cpp \
    ../Editors/ce5dishpackage.cpp \
    ../Editors/ce5dishpart1.cpp \
    ../Editors/ce5dishpart2.cpp \
    ../Editors/ce5dishremovereason.cpp \
    ../Editors/ce5editor.cpp \
    ../Editors/ce5goods.cpp \
    ../Editors/ce5goodsbarcodelabelview.cpp \
    ../Editors/ce5goodsclass.cpp \
    ../Editors/ce5goodsgroup.cpp \
    ../Editors/ce5goodsunit.cpp \
    ../Editors/ce5goodswaste.cpp \
    ../Editors/ce5halls.cpp \
    ../Editors/ce5menuname.cpp \
    ../Editors/ce5ordermark.cpp \
    ../Editors/ce5packagelist.cpp \
    ../Editors/ce5partner.cpp \
    ../Editors/ce5storage.cpp \
    ../Editors/ce5storereason.cpp \
    ../Editors/ce5table.cpp \
    ../Editors/ce5user.cpp \
    ../Editors/ce5usergroup.cpp \
    ../FrontDesk/barcode.cpp \
    ../FrontDesk/c5cashdoc.cpp \
    ../FrontDesk/c5changedocinputprice.cpp \
    ../FrontDesk/c5changepassword.cpp \
    ../FrontDesk/c5changepriceofgroup.cpp \
    ../FrontDesk/c5checkdatabase.cpp \
    ../FrontDesk/c5costumerdebtpayment.cpp \
    ../FrontDesk/c5datasynchronize.cpp \
    ../FrontDesk/c5daterange.cpp \
    ../FrontDesk/c5dbresetoption.cpp \
    ../FrontDesk/c5dbuseraccess.cpp \
    ../FrontDesk/c5dishgroupaction.cpp \
    ../FrontDesk/c5dishselfcostgenprice.cpp \
    ../FrontDesk/c5document.cpp \
    ../FrontDesk/c5filtervalues.cpp \
    ../FrontDesk/c5filterwidget.cpp \
    ../FrontDesk/c5goodsimage.cpp \
    ../FrontDesk/c5goodspricing.cpp \
    ../FrontDesk/c5grouppermissionseditor.cpp \
    ../FrontDesk/c5inputdate.cpp \
    ../FrontDesk/c5login.cpp \
    ../FrontDesk/c5mainwindow.cpp \
    ../FrontDesk/c5passwords.cpp \
    ../FrontDesk/c5progressdialog.cpp \
    ../FrontDesk/c5reportwidget.cpp \
    ../FrontDesk/c5salarydoc.cpp \
    ../FrontDesk/c5salefromstoreorder.cpp \
    ../FrontDesk/c5selector.cpp \
    ../FrontDesk/c5serviceconfig.cpp \
    ../FrontDesk/c5storebarcode.cpp \
    ../FrontDesk/c5storebarcodelist.cpp \
    ../FrontDesk/c5storedoc.cpp \
    ../FrontDesk/c5storeinventory.cpp \
    ../FrontDesk/c5toolbarwidget.cpp \
    ../FrontDesk/c5translatorform.cpp \
    ../FrontDesk/c5waiterorder.cpp \
    ../FrontDesk/dlgchangeoutputstore.cpp \
    ../Reports/c5settingswidget.cpp \
    ../Reports/cr5carvisits.cpp \
    ../Reports/cr5carvisitsfilter.cpp \
    ../Reports/cr5cashdetailed.cpp \
    ../Reports/cr5cashdetailedfilter.cpp \
    ../Reports/cr5cashmovement.cpp \
    ../Reports/cr5cashmovementfilter.cpp \
    ../Reports/cr5cashnames.cpp \
    ../Reports/cr5commonsales.cpp \
    ../Reports/cr5commonsalesfilter.cpp \
    ../Reports/cr5consumptionbysales.cpp \
    ../Reports/cr5consumptionbysalesdraft.cpp \
    ../Reports/cr5consumptionbysalesfilter.cpp \
    ../Reports/cr5consumptionbysalesfilterdraft.cpp \
    ../Reports/cr5consuptionreason.cpp \
    ../Reports/cr5consuptionreasonfilter.cpp \
    ../Reports/cr5costumerdebts.cpp \
    ../Reports/cr5costumerdebtsfilter.cpp \
    ../Reports/cr5creditcards.cpp \
    ../Reports/cr5databases.cpp \
    ../Reports/cr5debtstopartner.cpp \
    ../Reports/cr5discountstatisics.cpp \
    ../Reports/cr5discountstatisticsfilter.cpp \
    ../Reports/cr5discountsystem.cpp \
    ../Reports/cr5dish.cpp \
    ../Reports/cr5dishcomment.cpp \
    ../Reports/cr5dishpackage.cpp \
    ../Reports/cr5dishpart1.cpp \
    ../Reports/cr5dishpart2.cpp \
    ../Reports/cr5dishpriceselfcost.cpp \
    ../Reports/cr5dishpriceselfcostfilter.cpp \
    ../Reports/cr5dishremovereason.cpp \
    ../Reports/cr5documents.cpp \
    ../Reports/cr5documentsfilter.cpp \
    ../Reports/cr5goods.cpp \
    ../Reports/cr5goodsclasses.cpp \
    ../Reports/cr5goodsfilter.cpp \
    ../Reports/cr5goodsgroup.cpp \
    ../Reports/cr5goodsimages.cpp \
    ../Reports/cr5goodsmovement.cpp \
    ../Reports/cr5goodsmovementfilter.cpp \
    ../Reports/cr5goodspartners.cpp \
    ../Reports/cr5goodsstorages.cpp \
    ../Reports/cr5goodsunit.cpp \
    ../Reports/cr5goodswaste.cpp \
    ../Reports/cr5hall.cpp \
    ../Reports/cr5materialinstorefilter.cpp \
    ../Reports/cr5materialinstoreuncomplect.cpp \
    ../Reports/cr5materialinstoreuncomplectfilter.cpp \
    ../Reports/cr5materialmoveuncomplect.cpp \
    ../Reports/cr5materialmoveuncomplectfilter.cpp \
    ../Reports/cr5materialsinstore.cpp \
    ../Reports/cr5menunames.cpp \
    ../Reports/cr5menureview.cpp \
    ../Reports/cr5menureviewfilter.cpp \
    ../Reports/cr5menutranslator.cpp \
    ../Reports/cr5ordermarks.cpp \
    ../Reports/cr5preorders.cpp \
    ../Reports/cr5salarybyworkers.cpp \
    ../Reports/cr5salarybyworkersfilter.cpp \
    ../Reports/cr5saleandstorefilter.cpp \
    ../Reports/cr5saleflags.cpp \
    ../Reports/cr5salefromstore.cpp \
    ../Reports/cr5salefromstorefilter.cpp \
    ../Reports/cr5salefromstoretotal.cpp \
    ../Reports/cr5salefromstoretotalfilter.cpp \
    ../Reports/cr5saleremoveddishes.cpp \
    ../Reports/cr5saleremoveddishesfilter.cpp \
    ../Reports/cr5salesandstore.cpp \
    ../Reports/cr5salesbydishes.cpp \
    ../Reports/cr5salesbydishesfilter.cpp \
    ../Reports/cr5settings.cpp \
    ../Reports/cr5storedocuments.cpp \
    ../Reports/cr5storedocumentsfilter.cpp \
    ../Reports/cr5storereason.cpp \
    ../Reports/cr5tables.cpp \
    ../Reports/cr5tstoreextra.cpp \
    ../Reports/cr5tstoreextrafilter.cpp \
    ../Reports/cr5users.cpp \
    ../Reports/cr5usersfilter.cpp \
    ../Reports/cr5usersgroups.cpp \
    ../Shop/printreceipt.cpp \
    ../Shop/printreceiptgroup.cpp \
    ../Shop/selectprinters.cpp \
    c5printreceiptthread50mm.cpp \
    dlgcarnumber.cpp \
    dlgguests.cpp \
    dlglistdishspecial.cpp \
    dlglistofpackages.cpp \
    dlglistofshifts.cpp \
    dlgpreorder.cpp \
    dlgshiftrotation.cpp \
    dlgsplitorder.cpp \
    dlgtools.cpp \
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
    dlgpassword.cpp \
    dlgorder.cpp \
    ../Classes/c5user.cpp \
    ../Classes/c5menu.cpp \
    ../Classes/c5witerconf.cpp \
    ../Classes/c5waiterserver.cpp \
    ../Classes/c5dishtabledelegate.cpp \
    ../Classes/c5ordertabledelegate.cpp \
    ../Classes/c5halltabledelegate.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printservicethread.cpp \
    dlgpayment.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../../NewTax/Src/printtaxn.cpp \
    ../Controls/c5lineedit.cpp \
    ../Classes/c5printremovedservicethread.cpp \
    ../Controls/c5tablewidget.cpp \
    ../Controls/c5checkbox.cpp \
    ../Controls/c5combobox.cpp \
    ../Controls/c5dateedit.cpp \
    ../Controls/c5graphicsview.cpp \
    ../Classes/c5cache.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5widget.cpp \
    wguestorder.cpp \
    worder.cpp \
    wpaymentoptions.cpp \
    dlgcreditcardlist.cpp \
    dlgdishremovereason.cpp \
    dlglistofhall.cpp \
    dlglistofdishcomments.cpp \
    ../Classes/c5printjson.cpp \
    dlgreports.cpp \
    dlgreportslist.cpp \
    dlglistofmenu.cpp \
    ../Classes/c5part2tabledelegate.cpp \
    dlgguest.cpp \
    ../Classes/rkeyboard.cpp \
    dlgreceiptlanguage.cpp \
    dlgcl.cpp \
    ../Classes/c5logtoserverthread.cpp \
    ../Classes/c5permissions.cpp \
    dlgexitwithmessage.cpp \
    ../Classes/c5translator.cpp \
    c5printreceiptthread.cpp \
    ../Cafe5/dlgexitbyversion.cpp \
    ../Classes/fileversion.cpp \
    dlgsearchinmenu.cpp \
    ../Classes/c5waiterorderdoc.cpp

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
    ../Cafe5/c5cafecommon.h \
    ../Cafe5/c5double.h \
    ../Cafe5/c5license.h \
    ../Cafe5/c5licensedlg.h \
    ../Cafe5/c5logsystem.h \
    ../Cafe5/c5replication.h \
    ../Cafe5/calendar.h \
    ../Classes/barcode5.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5crypt.h \
    ../Classes/c5datedelegate.h \
    ../Classes/c5filelogwriter.h \
    ../Classes/c5grid.h \
    ../Classes/c5gridgilter.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5orderdriver.h \
    ../Classes/c5random.h \
    ../Classes/c5shoporder.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5tablerec.h \
    ../Classes/c5textdelegate.h \
    ../Classes/c5threadobject.h \
    ../Classes/c5userauth.h \
    ../Classes/calculator.h \
    ../Classes/checkforupdatethread.h \
    ../Classes/notificationwidget.h \
    ../Classes/proxytablewidgetdatabase.h \
    ../Controls/c5guicontrols.h \
    ../Controls/c5lineeditwithselector.h \
    ../DbData/datadriver.h \
    ../DbData/dbdata.h \
    ../DbData/dbhalls.h \
    ../DbData/dbtables.h \
    ../DbData/dbusers.h \
    ../Editors/c5cashname.h \
    ../Editors/c5dishwidget.h \
    ../Editors/c5editor.h \
    ../Editors/ce5createtablesforhall.h \
    ../Editors/ce5creditcard.h \
    ../Editors/ce5databases.h \
    ../Editors/ce5discountcard.h \
    ../Editors/ce5dishcomment.h \
    ../Editors/ce5dishpackage.h \
    ../Editors/ce5dishpart1.h \
    ../Editors/ce5dishpart2.h \
    ../Editors/ce5dishremovereason.h \
    ../Editors/ce5editor.h \
    ../Editors/ce5goods.h \
    ../Editors/ce5goodsbarcodelabelview.h \
    ../Editors/ce5goodsclass.h \
    ../Editors/ce5goodsgroup.h \
    ../Editors/ce5goodsunit.h \
    ../Editors/ce5goodswaste.h \
    ../Editors/ce5halls.h \
    ../Editors/ce5menuname.h \
    ../Editors/ce5ordermark.h \
    ../Editors/ce5packagelist.h \
    ../Editors/ce5partner.h \
    ../Editors/ce5storage.h \
    ../Editors/ce5storereason.h \
    ../Editors/ce5table.h \
    ../Editors/ce5user.h \
    ../Editors/ce5usergroup.h \
    ../FrontDesk/barcode.h \
    ../FrontDesk/c5cashdoc.h \
    ../FrontDesk/c5changedocinputprice.h \
    ../FrontDesk/c5changepassword.h \
    ../FrontDesk/c5changepriceofgroup.h \
    ../FrontDesk/c5checkdatabase.h \
    ../FrontDesk/c5costumerdebtpayment.h \
    ../FrontDesk/c5datasynchronize.h \
    ../FrontDesk/c5daterange.h \
    ../FrontDesk/c5dbresetoption.h \
    ../FrontDesk/c5dbuseraccess.h \
    ../FrontDesk/c5dishgroupaction.h \
    ../FrontDesk/c5dishselfcostgenprice.h \
    ../FrontDesk/c5document.h \
    ../FrontDesk/c5filtervalues.h \
    ../FrontDesk/c5filterwidget.h \
    ../FrontDesk/c5goodsimage.h \
    ../FrontDesk/c5goodspricing.h \
    ../FrontDesk/c5grouppermissionseditor.h \
    ../FrontDesk/c5inputdate.h \
    ../FrontDesk/c5login.h \
    ../FrontDesk/c5mainwindow.h \
    ../FrontDesk/c5passwords.h \
    ../FrontDesk/c5progressdialog.h \
    ../FrontDesk/c5reportwidget.h \
    ../FrontDesk/c5salarydoc.h \
    ../FrontDesk/c5salefromstoreorder.h \
    ../FrontDesk/c5selector.h \
    ../FrontDesk/c5serviceconfig.h \
    ../FrontDesk/c5storebarcode.h \
    ../FrontDesk/c5storebarcodelist.h \
    ../FrontDesk/c5storedoc.h \
    ../FrontDesk/c5storeinventory.h \
    ../FrontDesk/c5toolbarwidget.h \
    ../FrontDesk/c5translatorform.h \
    ../FrontDesk/c5waiterorder.h \
    ../FrontDesk/dlgchangeoutputstore.h \
    ../Reports/c5settingswidget.h \
    ../Reports/cr5carvisits.h \
    ../Reports/cr5carvisitsfilter.h \
    ../Reports/cr5cashdetailed.h \
    ../Reports/cr5cashdetailedfilter.h \
    ../Reports/cr5cashmovement.h \
    ../Reports/cr5cashmovementfilter.h \
    ../Reports/cr5cashnames.h \
    ../Reports/cr5commonsales.h \
    ../Reports/cr5commonsalesfilter.h \
    ../Reports/cr5consumptionbysales.h \
    ../Reports/cr5consumptionbysalesdraft.h \
    ../Reports/cr5consumptionbysalesfilter.h \
    ../Reports/cr5consumptionbysalesfilterdraft.h \
    ../Reports/cr5consuptionreason.h \
    ../Reports/cr5consuptionreasonfilter.h \
    ../Reports/cr5costumerdebts.h \
    ../Reports/cr5costumerdebtsfilter.h \
    ../Reports/cr5creditcards.h \
    ../Reports/cr5databases.h \
    ../Reports/cr5debtstopartner.h \
    ../Reports/cr5discountstatisics.h \
    ../Reports/cr5discountstatisticsfilter.h \
    ../Reports/cr5discountsystem.h \
    ../Reports/cr5dish.h \
    ../Reports/cr5dishcomment.h \
    ../Reports/cr5dishpackage.h \
    ../Reports/cr5dishpart1.h \
    ../Reports/cr5dishpart2.h \
    ../Reports/cr5dishpriceselfcost.h \
    ../Reports/cr5dishpriceselfcostfilter.h \
    ../Reports/cr5dishremovereason.h \
    ../Reports/cr5documents.h \
    ../Reports/cr5documentsfilter.h \
    ../Reports/cr5goods.h \
    ../Reports/cr5goodsclasses.h \
    ../Reports/cr5goodsfilter.h \
    ../Reports/cr5goodsgroup.h \
    ../Reports/cr5goodsimages.h \
    ../Reports/cr5goodsmovement.h \
    ../Reports/cr5goodsmovementfilter.h \
    ../Reports/cr5goodspartners.h \
    ../Reports/cr5goodsstorages.h \
    ../Reports/cr5goodsunit.h \
    ../Reports/cr5goodswaste.h \
    ../Reports/cr5hall.h \
    ../Reports/cr5materialinstorefilter.h \
    ../Reports/cr5materialinstoreuncomplect.h \
    ../Reports/cr5materialinstoreuncomplectfilter.h \
    ../Reports/cr5materialmoveuncomplect.h \
    ../Reports/cr5materialmoveuncomplectfilter.h \
    ../Reports/cr5materialsinstore.h \
    ../Reports/cr5menunames.h \
    ../Reports/cr5menureview.h \
    ../Reports/cr5menureviewfilter.h \
    ../Reports/cr5menutranslator.h \
    ../Reports/cr5ordermarks.h \
    ../Reports/cr5preorders.h \
    ../Reports/cr5salarybyworkers.h \
    ../Reports/cr5salarybyworkersfilter.h \
    ../Reports/cr5saleandstorefilter.h \
    ../Reports/cr5saleflags.h \
    ../Reports/cr5salefromstore.h \
    ../Reports/cr5salefromstorefilter.h \
    ../Reports/cr5salefromstoretotal.h \
    ../Reports/cr5salefromstoretotalfilter.h \
    ../Reports/cr5saleremoveddishes.h \
    ../Reports/cr5saleremoveddishesfilter.h \
    ../Reports/cr5salesandstore.h \
    ../Reports/cr5salesbydishes.h \
    ../Reports/cr5salesbydishesfilter.h \
    ../Reports/cr5settings.h \
    ../Reports/cr5storedocuments.h \
    ../Reports/cr5storedocumentsfilter.h \
    ../Reports/cr5storereason.h \
    ../Reports/cr5tables.h \
    ../Reports/cr5tstoreextra.h \
    ../Reports/cr5tstoreextrafilter.h \
    ../Reports/cr5users.h \
    ../Reports/cr5usersfilter.h \
    ../Reports/cr5usersgroups.h \
    ../Service/servicecommands.h \
    ../Shop/printreceipt.h \
    ../Shop/printreceiptgroup.h \
    ../Shop/selectprinters.h \
    c5printreceiptthread50mm.h \
    dlgcarnumber.h \
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
    dlglistdishspecial.h \
    dlglistofpackages.h \
    dlglistofshifts.h \
    dlgpassword.h \
    dlgorder.h \
    ../Classes/c5user.h \
    ../Classes/c5menu.h \
    ../Classes/c5witerconf.h \
    ../Classes/c5waiterserver.h \
    ../Classes/c5dishtabledelegate.h \
    ../Classes/c5ordertabledelegate.h \
    ../Classes/c5halltabledelegate.h \
    ../Classes/c5printing.h \
    ../Classes/c5printservicethread.h \
    dlgpayment.h \
    ../Classes/QRCodeGenerator.h \
    ../../NewTax/Src/printtaxn.h \
    ../Controls/c5lineedit.h \
    ../Classes/c5printremovedservicethread.h \
    dlgpreorder.h \
    dlgshiftrotation.h \
    dlgsplitorder.h \
    dlgtools.h \
    rc.h \
    res.rc \
    ../Controls/c5tablewidget.h \
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
    wguestorder.h \
    worder.h \
    wpaymentoptions.h \
    dlgcreditcardlist.h \
    dlgdishremovereason.h \
    dlglistofhall.h \
    dlglistofdishcomments.h \
    ../Classes/c5printjson.h \
    dlgreports.h \
    dlgreportslist.h \
    dlglistofmenu.h \
    ../Classes/c5part2tabledelegate.h \
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
    dlgsearchinmenu.h \
    ../Classes/c5waiterorderdoc.h

FORMS += \
    ../Cafe5/c5licensedlg.ui \
    ../Cafe5/calendar.ui \
    ../Classes/c5grid.ui \
    ../Classes/c5gridgilter.ui \
    ../Classes/calculator.ui \
    ../Classes/notificationwidget.ui \
    ../Editors/c5cashname.ui \
    ../Editors/c5dishwidget.ui \
    ../Editors/c5editor.ui \
    ../Editors/ce5createtablesforhall.ui \
    ../Editors/ce5creditcard.ui \
    ../Editors/ce5databases.ui \
    ../Editors/ce5discountcard.ui \
    ../Editors/ce5dishcomment.ui \
    ../Editors/ce5dishpackage.ui \
    ../Editors/ce5dishpart1.ui \
    ../Editors/ce5dishpart2.ui \
    ../Editors/ce5dishremovereason.ui \
    ../Editors/ce5goods.ui \
    ../Editors/ce5goodsclass.ui \
    ../Editors/ce5goodsgroup.ui \
    ../Editors/ce5goodsunit.ui \
    ../Editors/ce5goodswaste.ui \
    ../Editors/ce5halls.ui \
    ../Editors/ce5menuname.ui \
    ../Editors/ce5ordermark.ui \
    ../Editors/ce5packagelist.ui \
    ../Editors/ce5partner.ui \
    ../Editors/ce5storage.ui \
    ../Editors/ce5storereason.ui \
    ../Editors/ce5table.ui \
    ../Editors/ce5user.ui \
    ../Editors/ce5usergroup.ui \
    ../FrontDesk/c5cashdoc.ui \
    ../FrontDesk/c5changedocinputprice.ui \
    ../FrontDesk/c5changepassword.ui \
    ../FrontDesk/c5changepriceofgroup.ui \
    ../FrontDesk/c5checkdatabase.ui \
    ../FrontDesk/c5costumerdebtpayment.ui \
    ../FrontDesk/c5datasynchronize.ui \
    ../FrontDesk/c5daterange.ui \
    ../FrontDesk/c5dbresetoption.ui \
    ../FrontDesk/c5dbuseraccess.ui \
    ../FrontDesk/c5dishgroupaction.ui \
    ../FrontDesk/c5dishselfcostgenprice.ui \
    ../FrontDesk/c5filtervalues.ui \
    ../FrontDesk/c5goodsimage.ui \
    ../FrontDesk/c5goodspricing.ui \
    ../FrontDesk/c5grouppermissionseditor.ui \
    ../FrontDesk/c5inputdate.ui \
    ../FrontDesk/c5login.ui \
    ../FrontDesk/c5mainwindow.ui \
    ../FrontDesk/c5passwords.ui \
    ../FrontDesk/c5progressdialog.ui \
    ../FrontDesk/c5salarydoc.ui \
    ../FrontDesk/c5salefromstoreorder.ui \
    ../FrontDesk/c5selector.ui \
    ../FrontDesk/c5serviceconfig.ui \
    ../FrontDesk/c5storebarcode.ui \
    ../FrontDesk/c5storebarcodelist.ui \
    ../FrontDesk/c5storedoc.ui \
    ../FrontDesk/c5storeinventory.ui \
    ../FrontDesk/c5toolbarwidget.ui \
    ../FrontDesk/c5translatorform.ui \
    ../FrontDesk/c5waiterorder.ui \
    ../FrontDesk/dlgchangeoutputstore.ui \
    ../Reports/c5settingswidget.ui \
    ../Reports/cr5carvisitsfilter.ui \
    ../Reports/cr5cashdetailedfilter.ui \
    ../Reports/cr5cashmovementfilter.ui \
    ../Reports/cr5commonsalesfilter.ui \
    ../Reports/cr5consumptionbysalesfilter.ui \
    ../Reports/cr5consumptionbysalesfilterdraft.ui \
    ../Reports/cr5consuptionreasonfilter.ui \
    ../Reports/cr5costumerdebtsfilter.ui \
    ../Reports/cr5discountstatisticsfilter.ui \
    ../Reports/cr5dishpriceselfcostfilter.ui \
    ../Reports/cr5documentsfilter.ui \
    ../Reports/cr5goodsfilter.ui \
    ../Reports/cr5goodsimages.ui \
    ../Reports/cr5goodsmovementfilter.ui \
    ../Reports/cr5materialinstorefilter.ui \
    ../Reports/cr5materialinstoreuncomplectfilter.ui \
    ../Reports/cr5materialmoveuncomplectfilter.ui \
    ../Reports/cr5menureviewfilter.ui \
    ../Reports/cr5menutranslator.ui \
    ../Reports/cr5salarybyworkersfilter.ui \
    ../Reports/cr5saleandstorefilter.ui \
    ../Reports/cr5saleflags.ui \
    ../Reports/cr5salefromstorefilter.ui \
    ../Reports/cr5salefromstoretotalfilter.ui \
    ../Reports/cr5saleremoveddishesfilter.ui \
    ../Reports/cr5salesbydishesfilter.ui \
    ../Reports/cr5storedocumentsfilter.ui \
    ../Reports/cr5tstoreextrafilter.ui \
    ../Reports/cr5usersfilter.ui \
    ../Shop/selectprinters.ui \
    dlgcarnumber.ui \
        dlgface.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    dlgguests.ui \
    dlglistdishspecial.ui \
    dlglistofpackages.ui \
    dlglistofshifts.ui \
    dlgpassword.ui \
    dlgorder.ui \
    dlgpayment.ui \
    ../Classes/c5printpreview.ui \
    dlgpreorder.ui \
    dlgshiftrotation.ui \
    dlgsplitorder.ui \
    dlgtools.ui \
    wguestorder.ui \
    worder.ui \
    wpaymentoptions.ui \
    dlgcreditcardlist.ui \
    dlgdishremovereason.ui \
    dlglistofhall.ui \
    dlglistofdishcomments.ui \
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
INCLUDEPATH += ../FrontDesk
INCLUDEPATH += ../Reports
INCLUDEPATH += ../Service
INCLUDEPATH += ../DbData
INCLUDEPATH += ../Shop
INCLUDEPATH += ../Editors
INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += ../Editors
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include/openssl
INCLUDEPATH += C:/Projects/XLSX/src

RESOURCES += \
    ../resources/res.qrc \
    ../resources/resources.qrc \
    translator.qrc

DISTFILES += \
    cup.ico

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/Soft/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto


win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings
