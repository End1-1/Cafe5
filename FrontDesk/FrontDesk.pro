#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T15:35:15
#
#-------------------------------------------------

QT       += core gui network printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_FILE = res.rc

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

TARGET = FrontDesk
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += _DBDRIVER_=\\\"QMYSQL\\\"
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"FRONTDESK\\\"
DEFINES += FRONTDESK

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../Cafe5/c5cafecommon.cpp \
    ../Cafe5/c5double.cpp \
    ../Cafe5/c5license.cpp \
    ../Cafe5/c5licensedlg.cpp \
    ../Classes/c5crypt.cpp \
    ../Classes/c5filelogwriter.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/c5threadobject.cpp \
    ../Classes/notificationwidget.cpp \
    ../Classes/proxytablewidgetdatabase.cpp \
    ../Editors/c5cashname.cpp \
    ../Editors/ce5dishpackage.cpp \
    ../Editors/ce5goodsbarcodelabelview.cpp \
    ../Editors/ce5goodsclass.cpp \
    ../Editors/ce5packagelist.cpp \
    ../Editors/ce5storereason.cpp \
    ../Reports/cr5carvisits.cpp \
    ../Reports/cr5carvisitsfilter.cpp \
    ../Reports/cr5cashdetailed.cpp \
    ../Reports/cr5cashdetailedfilter.cpp \
    ../Reports/cr5cashnames.cpp \
    ../Reports/cr5consuptionreason.cpp \
    ../Reports/cr5consuptionreasonfilter.cpp \
    ../Reports/cr5costumerdebts.cpp \
    ../Reports/cr5costumerdebtsfilter.cpp \
    ../Reports/cr5debtstopartner.cpp \
    ../Reports/cr5discountstatisics.cpp \
    ../Reports/cr5discountstatisticsfilter.cpp \
    ../Reports/cr5dishpackage.cpp \
    ../Reports/cr5goodsclasses.cpp \
    ../Reports/cr5goodsfilter.cpp \
    ../Reports/cr5menureview.cpp \
    ../Reports/cr5menureviewfilter.cpp \
    ../Reports/cr5salarybyworkers.cpp \
    ../Reports/cr5salarybyworkersfilter.cpp \
    ../Reports/cr5salefromstoretotal.cpp \
    ../Reports/cr5salefromstoretotalfilter.cpp \
    ../Reports/cr5saleremoveddishes.cpp \
    ../Reports/cr5saleremoveddishesfilter.cpp \
    ../Reports/cr5salesbydishes.cpp \
    ../Reports/cr5salesbydishesfilter.cpp \
    ../Reports/cr5storereason.cpp \
    barcode.cpp \
    c5cashdoc.cpp \
    c5costumerdebtpayment.cpp \
    c5dishgroupaction.cpp \
    c5document.cpp \
    c5inputdate.cpp \
    c5progressdialog.cpp \
    c5salarydoc.cpp \
    c5serviceconfig.cpp \
    c5storebarcode.cpp \
    c5storebarcodelist.cpp \
    dlgchangeoutputstore.cpp \
        main.cpp \
        c5mainwindow.cpp \
    ../Cafe5/c5config.cpp \
    ../Cafe5/c5connection.cpp \
    ../Cafe5/c5database.cpp \
    ../Cafe5/c5dialog.cpp \
    ../Cafe5/c5message.cpp \
    ../Cafe5/c5utils.cpp \
    ../Cafe5/c5sockethandler.cpp \
    c5login.cpp \
    ../Classes/c5permissions.cpp \
    c5reportwidget.cpp \
    ../Classes/c5grid.cpp \
    ../Reports/cr5usersgroups.cpp \
    ../Reports/cr5users.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5textdelegate.cpp \
    ../Classes/c5combodelegate.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5cache.cpp \
    ../Controls/c5lineedit.cpp \
    ../Controls/c5combobox.cpp \
    c5filtervalues.cpp \
    ../Reports/cr5commonsales.cpp \
    ../Classes/c5gridgilter.cpp \
    c5filterwidget.cpp \
    ../Reports/cr5commonsalesfilter.cpp \
    ../Controls/c5dateedit.cpp \
    c5passwords.cpp \
    ../Classes/c5widget.cpp \
    c5grouppermissionseditor.cpp \
    ../Controls/c5checkbox.cpp \
    ../Reports/cr5databases.cpp \
    ../Reports/cr5dishpart1.cpp \
    ../Reports/cr5dishpart2.cpp \
    ../Reports/cr5dish.cpp \
    ../Reports/cr5settings.cpp \
    ../Reports/c5settingswidget.cpp \
    ../Reports/cr5goodsunit.cpp \
    ../Reports/cr5goodsgroup.cpp \
    ../Reports/cr5goods.cpp \
    ../Reports/cr5goodswaste.cpp \
    c5selector.cpp \
    ../Controls/c5lineeditwithselector.cpp \
    c5storedoc.cpp \
    ../Reports/cr5goodsstorages.cpp \
    ../Reports/cr5goodspartners.cpp \
    ../Controls/c5tablewidget.cpp \
    ../Reports/cr5documents.cpp \
    ../Reports/cr5materialsinstore.cpp \
    ../Reports/cr5materialinstorefilter.cpp \
    ../Reports/cr5goodsmovement.cpp \
    ../Reports/cr5goodsmovementfilter.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5printing.cpp \
    ../Controls/c5graphicsview.cpp \
    c5storeinventory.cpp \
    ../Reports/cr5tstoreextra.cpp \
    ../Reports/cr5tstoreextrafilter.cpp \
    ../Reports/cr5menunames.cpp \
    ../Classes/c5datedelegate.cpp \
    ../Reports/cr5documentsfilter.cpp \
    ../Reports/cr5consumptionbysales.cpp \
    ../Reports/cr5consumptionbysalesfilter.cpp \
    ../Editors/ce5partner.cpp \
    ../Editors/c5editor.cpp \
    ../Editors/ce5editor.cpp \
    ../Editors/ce5goodsunit.cpp \
    ../Editors/ce5storage.cpp \
    ../Editors/ce5goods.cpp \
    ../Editors/ce5dishpart1.cpp \
    ../Editors/ce5dishpart2.cpp \
    ../Editors/c5dishwidget.cpp \
    ../Editors/ce5menuname.cpp \
    ../Editors/ce5goodsgroup.cpp \
    ../Editors/ce5goodswaste.cpp \
    c5welcomepage.cpp \
    ../Editors/ce5usergroup.cpp \
    ../Editors/ce5user.cpp \
    ../Reports/cr5creditcards.cpp \
    ../Editors/ce5creditcard.cpp \
    ../Reports/cr5dishremovereason.cpp \
    ../Editors/ce5dishremovereason.cpp \
    ../Reports/cr5discountsystem.cpp \
    ../Editors/ce5discountcard.cpp \
    ../Editors/ce5dishcomment.cpp \
    ../Reports/cr5dishcomment.cpp \
    ../Reports/cr5salefromstore.cpp \
    ../Reports/cr5salefromstorefilter.cpp \
    c5salefromstoreorder.cpp \
    c5datasynchronize.cpp \
    ../Editors/ce5databases.cpp \
    ../Reports/cr5hall.cpp \
    ../Editors/ce5halls.cpp \
    ../Reports/cr5tables.cpp \
    ../Editors/ce5table.cpp \
    ../Editors/ce5createtablesforhall.cpp \
    c5dbuseraccess.cpp \
    ../Reports/cr5menutranslator.cpp \
    c5changepassword.cpp \
    ../Reports/cr5dishpriceselfcost.cpp \
    c5dishselfcostgenprice.cpp \
    ../Reports/cr5dishpriceselfcostfilter.cpp \
    ../Reports/cr5storedocuments.cpp \
    ../Reports/cr5storedocumentsfilter.cpp \
    c5translatorform.cpp \
    c5toolbarwidget.cpp \
    ../Classes/checkforupdatethread.cpp \
    ../Classes/fileversion.cpp \
    c5waiterorder.cpp \
    ../Classes/c5waiterorderdoc.cpp \
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
    ../../XLSX/src/xlsxwriter.cpp

HEADERS += \
    ../Cafe5/c5cafecommon.h \
    ../Cafe5/c5double.h \
    ../Cafe5/c5license.h \
    ../Cafe5/c5licensedlg.h \
    ../Classes/barcode5.h \
    ../Classes/c5crypt.h \
    ../Classes/c5filelogwriter.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/c5threadobject.h \
    ../Classes/notificationwidget.h \
    ../Classes/proxytablewidgetdatabase.h \
    ../Editors/c5cashname.h \
    ../Editors/ce5dishpackage.h \
    ../Editors/ce5goodsbarcodelabelview.h \
    ../Editors/ce5goodsclass.h \
    ../Editors/ce5packagelist.h \
    ../Editors/ce5storereason.h \
    ../Reports/cr5carvisits.h \
    ../Reports/cr5carvisitsfilter.h \
    ../Reports/cr5cashdetailed.h \
    ../Reports/cr5cashdetailedfilter.h \
    ../Reports/cr5cashnames.h \
    ../Reports/cr5consuptionreason.h \
    ../Reports/cr5consuptionreasonfilter.h \
    ../Reports/cr5costumerdebts.h \
    ../Reports/cr5costumerdebtsfilter.h \
    ../Reports/cr5debtstopartner.h \
    ../Reports/cr5discountstatisics.h \
    ../Reports/cr5discountstatisticsfilter.h \
    ../Reports/cr5dishpackage.h \
    ../Reports/cr5goodsclasses.h \
    ../Reports/cr5goodsfilter.h \
    ../Reports/cr5menureview.h \
    ../Reports/cr5menureviewfilter.h \
    ../Reports/cr5salarybyworkers.h \
    ../Reports/cr5salarybyworkersfilter.h \
    ../Reports/cr5salefromstoretotal.h \
    ../Reports/cr5salefromstoretotalfilter.h \
    ../Reports/cr5saleremoveddishes.h \
    ../Reports/cr5saleremoveddishesfilter.h \
    ../Reports/cr5salesbydishes.h \
    ../Reports/cr5salesbydishesfilter.h \
    ../Reports/cr5storereason.h \
    ../Service/servicecommands.h \
    barcode.h \
    c5cashdoc.h \
    c5costumerdebtpayment.h \
    c5dishgroupaction.h \
    c5document.h \
    c5inputdate.h \
        c5mainwindow.h \
    ../Cafe5/c5config.h \
    ../Cafe5/c5connection.h \
    ../Cafe5/c5database.h \
    ../Cafe5/c5dialog.h \
    ../Cafe5/c5message.h \
    ../Cafe5/c5utils.h \
    ../Cafe5/c5sockethandler.h \
    c5login.h \
    ../Classes/c5permissions.h \
    c5progressdialog.h \
    c5reportwidget.h \
    ../Classes/c5grid.h \
    ../Reports/cr5usersgroups.h \
    ../Reports/cr5users.h \
    ../Classes/c5tablemodel.h \
    ../Classes/c5textdelegate.h \
    ../Classes/c5combodelegate.h \
    ../Classes/c5tableview.h \
    ../Classes/c5cache.h \
    ../Controls/c5lineedit.h \
    ../Controls/c5combobox.h \
    c5filtervalues.h \
    ../Reports/cr5commonsales.h \
    ../Classes/c5gridgilter.h \
    c5filterwidget.h \
    ../Reports/cr5commonsalesfilter.h \
    ../Controls/c5dateedit.h \
    c5passwords.h \
    ../Classes/c5widget.h \
    c5grouppermissionseditor.h \
    ../Controls/c5checkbox.h \
    ../Reports/cr5databases.h \
    ../Reports/cr5dishpart1.h \
    ../Reports/cr5dishpart2.h \
    ../Reports/cr5dish.h \
    ../Reports/cr5settings.h \
    ../Reports/c5settingswidget.h \
    ../Reports/cr5goodsunit.h \
    ../Reports/cr5goodsgroup.h \
    ../Reports/cr5goods.h \
    ../Reports/cr5goodswaste.h \
    c5salarydoc.h \
    c5selector.h \
    ../Controls/c5lineeditwithselector.h \
    c5serviceconfig.h \
    c5storebarcode.h \
    c5storebarcodelist.h \
    c5storedoc.h \
    ../Reports/cr5goodsstorages.h \
    ../Reports/cr5goodspartners.h \
    ../Controls/c5tablewidget.h \
    ../Reports/cr5documents.h \
    ../Reports/cr5materialsinstore.h \
    ../Reports/cr5materialinstorefilter.h \
    ../Reports/cr5goodsmovement.h \
    ../Reports/cr5goodsmovementfilter.h \
    ../Classes/c5printpreview.h \
    ../Classes/c5printing.h \
    ../Controls/c5graphicsview.h \
    c5storeinventory.h \
    ../Reports/cr5tstoreextra.h \
    ../Reports/cr5tstoreextrafilter.h \
    ../Reports/cr5menunames.h \
    ../Classes/c5datedelegate.h \
    ../Reports/cr5documentsfilter.h \
    ../Reports/cr5consumptionbysales.h \
    ../Reports/cr5consumptionbysalesfilter.h \
    dlgchangeoutputstore.h \
    rc.h \
    res.rc \
    ../Editors/ce5partner.h \
    ../Editors/c5editor.h \
    ../Editors/ce5editor.h \
    ../Editors/ce5goodsunit.h \
    ../Editors/ce5storage.h \
    ../Editors/ce5goods.h \
    ../Editors/ce5dishpart1.h \
    ../Editors/ce5dishpart2.h \
    ../Editors/c5dishwidget.h \
    ../Editors/ce5menuname.h \
    ../Editors/ce5goodsgroup.h \
    ../Editors/ce5goodswaste.h \
    c5welcomepage.h \
    ../Editors/ce5usergroup.h \
    ../Editors/ce5user.h \
    ../Reports/cr5creditcards.h \
    ../Editors/ce5creditcard.h \
    ../Reports/cr5dishremovereason.h \
    ../Editors/ce5dishremovereason.h \
    ../Reports/cr5discountsystem.h \
    ../Editors/ce5discountcard.h \
    ../Editors/ce5dishcomment.h \
    ../Reports/cr5dishcomment.h \
    ../Reports/cr5salefromstore.h \
    ../Reports/cr5salefromstorefilter.h \
    c5salefromstoreorder.h \
    c5datasynchronize.h \
    ../Editors/ce5databases.h \
    ../Reports/cr5hall.h \
    ../Editors/ce5halls.h \
    ../Reports/cr5tables.h \
    ../Editors/ce5table.h \
    ../Editors/ce5createtablesforhall.h \
    c5dbuseraccess.h \
    ../Reports/cr5menutranslator.h \
    c5changepassword.h \
    ../Reports/cr5dishpriceselfcost.h \
    c5dishselfcostgenprice.h \
    ../Reports/cr5dishpriceselfcostfilter.h \
    ../Reports/cr5storedocuments.h \
    ../Reports/cr5storedocumentsfilter.h \
    c5translatorform.h \
    c5toolbarwidget.h \
    ../Classes/checkforupdatethread.h \
    ../Classes/fileversion.h \
    c5waiterorder.h \
    ../Classes/c5waiterorderdoc.h \
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
    ../../XLSX/src/zip.h

FORMS += \
    ../Cafe5/c5licensedlg.ui \
    ../Classes/notificationwidget.ui \
    ../Editors/c5cashname.ui \
    ../Editors/ce5dishpackage.ui \
    ../Editors/ce5goodsclass.ui \
    ../Editors/ce5packagelist.ui \
    ../Editors/ce5storereason.ui \
    ../Reports/cr5carvisitsfilter.ui \
    ../Reports/cr5cashdetailedfilter.ui \
    ../Reports/cr5consuptionreasonfilter.ui \
    ../Reports/cr5costumerdebtsfilter.ui \
    ../Reports/cr5discountstatisticsfilter.ui \
    ../Reports/cr5goodsfilter.ui \
    ../Reports/cr5menureviewfilter.ui \
    ../Reports/cr5salarybyworkersfilter.ui \
    ../Reports/cr5salefromstoretotalfilter.ui \
    ../Reports/cr5saleremoveddishesfilter.ui \
    ../Reports/cr5salesbydishesfilter.ui \
    c5cashdoc.ui \
    c5costumerdebtpayment.ui \
    c5dishgroupaction.ui \
    c5inputdate.ui \
        c5mainwindow.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    c5login.ui \
    ../Classes/c5grid.ui \
    c5filtervalues.ui \
    ../Classes/c5gridgilter.ui \
    ../Reports/cr5commonsalesfilter.ui \
    c5passwords.ui \
    c5grouppermissionseditor.ui \
    ../Reports/c5settingswidget.ui \
    c5progressdialog.ui \
    c5salarydoc.ui \
    c5selector.ui \
    c5serviceconfig.ui \
    c5storebarcode.ui \
    c5storebarcodelist.ui \
    c5storedoc.ui \
    ../Reports/cr5materialinstorefilter.ui \
    ../Reports/cr5goodsmovementfilter.ui \
    ../Classes/c5printpreview.ui \
    c5storeinventory.ui \
    ../Reports/cr5tstoreextrafilter.ui \
    ../Reports/cr5documentsfilter.ui \
    ../Reports/cr5consumptionbysalesfilter.ui \
    ../Editors/ce5partner.ui \
    ../Editors/c5editor.ui \
    ../Editors/ce5goodsunit.ui \
    ../Editors/ce5storage.ui \
    ../Editors/ce5goods.ui \
    ../Editors/ce5dishpart1.ui \
    ../Editors/ce5dishpart2.ui \
    ../Editors/c5dishwidget.ui \
    ../Editors/ce5menuname.ui \
    ../Editors/ce5goodsgroup.ui \
    ../Editors/ce5goodswaste.ui \
    c5welcomepage.ui \
    ../Editors/ce5usergroup.ui \
    ../Editors/ce5user.ui \
    ../Editors/ce5creditcard.ui \
    ../Editors/ce5dishremovereason.ui \
    ../Editors/ce5discountcard.ui \
    ../Editors/ce5dishcomment.ui \
    ../Reports/cr5salefromstorefilter.ui \
    c5salefromstoreorder.ui \
    c5datasynchronize.ui \
    ../Editors/ce5databases.ui \
    ../Editors/ce5halls.ui \
    ../Editors/ce5table.ui \
    ../Editors/ce5createtablesforhall.ui \
    c5dbuseraccess.ui \
    ../Reports/cr5menutranslator.ui \
    c5changepassword.ui \
    c5dishselfcostgenprice.ui \
    ../Reports/cr5dishpriceselfcostfilter.ui \
    ../Reports/cr5storedocumentsfilter.ui \
    c5translatorform.ui \
    c5toolbarwidget.ui \
    c5waiterorder.ui \
    dlgchangeoutputstore.ui

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Cache
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Reports
INCLUDEPATH += ../Controls
INCLUDEPATH += ../Editors
INCLUDEPATH += ../Waiter
INCLUDEPATH += ../RESOURCES
INCLUDEPATH += ../Service
INCLUDEPATH += c:/projects/xlsx/src
INCLUDEPATH += C:/OpenSSL-Win32/include
INCLUDEPATH += C:/OpenSSL-Win32/include/openssl

RESOURCES += \
    ../resources/res.qrc \
    lang.qrc

#QMAKE_CXXFLAGS += -Werror

DISTFILES += \
    storehouse.ico

ICON = storehouse.ico

LIBS += -lVersion
LIBS += -LC:/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto
