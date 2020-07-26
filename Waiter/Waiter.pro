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
    ../Cafe5/c5license.cpp \
    ../Classes/c5crypt.cpp \
    ../Classes/c5jsondb.cpp \
    ../Classes/c5storedraftwriter.cpp \
    ../Classes/notificationwidget.cpp \
    c5lineeditwithselector.cpp \
    c5printreceiptthread50mm.cpp \
    dlgcarnumber.cpp \
    dlglistdishspecial.cpp \
    dlglistofpackages.cpp \
    dlglistofshifts.cpp \
    dlgshiftrotation.cpp \
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
    ../Cafe5/c5license.h \
    ../Classes/c5crypt.h \
    ../Classes/c5jsondb.h \
    ../Classes/c5storedraftwriter.h \
    ../Classes/notificationwidget.h \
    c5lineeditwithselector.h \
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
    dlgshiftrotation.h \
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
    ../Classes/notificationwidget.ui \
    dlgcarnumber.ui \
        dlgface.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    dlglistdishspecial.ui \
    dlglistofpackages.ui \
    dlglistofshifts.ui \
    dlgpassword.ui \
    dlgorder.ui \
    dlgpayment.ui \
    ../Classes/c5printpreview.ui \
    dlgshiftrotation.ui \
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
