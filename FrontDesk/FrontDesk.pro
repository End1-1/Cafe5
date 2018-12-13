#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T15:35:15
#
#-------------------------------------------------

QT       += core gui network printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_FILE = res.rc

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
    ../Cafe5/excel.cpp \
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
    ../Editors/ce5client.cpp

HEADERS += \
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
    ../Cafe5/excel.h \
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
    c5selector.h \
    ../Controls/c5lineeditwithselector.h \
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
    ../Editors/ce5client.h

FORMS += \
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
    c5selector.ui \
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
    ../Editors/ce5client.ui

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Reports
INCLUDEPATH += ../Controls
INCLUDEPATH += ../Editors
INCLUDEPATH += ../RESOURCES

RESOURCES += \
    ../resources/res.qrc \
    lang.qrc

QMAKE_CXXFLAGS += -Werror

DISTFILES += \
    LastCache \
    storehouse.ico

ICON = storehouse.ico
