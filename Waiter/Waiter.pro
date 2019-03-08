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

QMAKE_CXXFLAGS += -Werror

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
    ../Classes/c5order.cpp \
    ../Classes/c5waiterserver.cpp \
    ../Classes/c5dishtabledelegate.cpp \
    ../Classes/c5ordertabledelegate.cpp \
    ../Classes/c5halltabledelegate.cpp \
    ../Classes/c5printing.cpp \
    ../Classes/c5printservicethread.cpp \
    dlgpayment.cpp \
    ../Classes/c5printreceiptthread.cpp \
    ../Classes/QRCodeGenerator.cpp \
    ../../NewTax/printtaxn.cpp \
    ../Controls/c5lineedit.cpp \
    ../Classes/c5printremovedservicethread.cpp \
    ../Controls/c5tablewidget.cpp \
    ../Controls/c5checkbox.cpp \
    ../Controls/c5combobox.cpp \
    ../Controls/c5dateedit.cpp \
    ../Controls/c5graphicsview.cpp \
    ../Controls/c5lineeditwithselector.cpp \
    ../Cafe5/excel.cpp \
    ../Classes/c5cache.cpp \
    ../FrontDesk/c5selector.cpp \
    ../Classes/c5grid.cpp \
    ../Editors/c5editor.cpp \
    ../Editors/ce5editor.cpp \
    ../Classes/c5tablemodel.cpp \
    ../Classes/c5tableview.cpp \
    ../Classes/c5printpreview.cpp \
    ../Classes/c5widget.cpp \
    ../Classes/c5gridgilter.cpp \
    ../FrontDesk/c5filtervalues.cpp \
    wpaymentoptions.cpp \
    c5cafecommon.cpp \
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
    dlgcl.cpp

HEADERS += \
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
    dlgpassword.h \
    dlgorder.h \
    ../Classes/c5user.h \
    ../Classes/c5menu.h \
    ../Classes/c5witerconf.h \
    ../Classes/c5order.h \
    ../Classes/c5waiterserver.h \
    ../Classes/c5dishtabledelegate.h \
    ../Classes/c5ordertabledelegate.h \
    ../Classes/c5halltabledelegate.h \
    ../Classes/c5printing.h \
    ../Classes/c5printservicethread.h \
    dlgpayment.h \
    ../Classes/c5printreceiptthread.h \
    ../Classes/QRCodeGenerator.h \
    ../../NewTax/printtaxn.h \
    ../Controls/c5lineedit.h \
    ../Classes/c5printremovedservicethread.h \
    rc.h \
    res.rc \
    ../Controls/c5tablewidget.h \
    ../Controls/c5checkbox.h \
    ../Controls/c5combobox.h \
    ../Controls/c5dateedit.h \
    ../Controls/c5graphicsview.h \
    ../Controls/c5lineeditwithselector.h \
    ../Cafe5/excel.h \
    ../FrontDesk/c5selector.h \
    ../Classes/c5grid.h \
    ../Editors/c5editor.h \
    ../Editors/ce5editor.h \
    ../Classes/c5tablemodel.h \
    ../Classes/c5tableview.h \
    ../Classes/c5printpreview.h \
    ../Classes/c5widget.h \
    ../Classes/c5gridgilter.h \
    ../FrontDesk/c5filtervalues.h \
    ../Classes/c5cache.h \
    wpaymentoptions.h \
    c5cafecommon.h \
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
    dlgcl.h

FORMS += \
        dlgface.ui \
    ../Cafe5/c5connection.ui \
    ../Cafe5/c5message.ui \
    dlgpassword.ui \
    dlgorder.ui \
    dlgpayment.ui \
    ../FrontDesk/c5selector.ui \
    ../Classes/c5grid.ui \
    ../Editors/c5editor.ui \
    ../Classes/c5printpreview.ui \
    ../Classes/c5gridgilter.ui \
    ../FrontDesk/c5filtervalues.ui \
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
    dlgcl.ui

INCLUDEPATH += ../Cafe5
INCLUDEPATH += ../Classes
INCLUDEPATH += ../Controls
INCLUDEPATH += ../FrontDesk
INCLUDEPATH += ../../NewTax
INCLUDEPATH += ../Editors
INCLUDEPATH += C:/OpenSSL-Win32.0/include
INCLUDEPATH += C:/OpenSSL-Win32.0/include/openssl

RESOURCES += \
    ../resources/resources.qrc \
    translator.qrc

DISTFILES += \
    cup.ico

LIBS += -lVersion
LIBS += -lwsock32
LIBS += -LC:/OpenSSL-Win32/lib
LIBS += -lopenssl
LIBS += -llibcrypto
