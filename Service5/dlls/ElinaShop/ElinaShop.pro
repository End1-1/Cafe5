QT += sql network widgets printsupport

TEMPLATE = lib
DEFINES += ELINASHOP_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../../NewTax/Src/printtaxn.cpp \
    ../../../Classes/c5printing.cpp \
    ../../../Classes/notificationwidget.cpp \
    ../../handlers/httpheader.cpp \
    ../../handlers/requesthandler.cpp \
    ../../handlers/socketdata.cpp \
    ../../utils/database.cpp \
    ../../utils/jsonhandler.cpp \
    ../../utils/logwriter.cpp \
    elinashop.cpp \
    printreceiptgroup.cpp \
    shopmanager.cpp \
    storemanager.cpp

HEADERS += \
    ../../../../NewTax/Src/printtaxn.h \
    ../../../Classes/c5printing.h \
    ../../../Classes/notificationwidget.h \
    ../../handlers/httpheader.h \
    ../../handlers/requesthandler.h \
    ../../handlers/socketdata.h \
    ../../utils/database.h \
    ../../utils/jsonhandler.h \
    ../../utils/logwriter.h \
    elinashop.h \
    printreceiptgroup.h \
    shopmanager.h \
    storemanager.h

TRANSLATIONS += \
    ElinaShop_hy_AM.ts

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += C:/Projects/Cafe5/Service5/handlers
INCLUDEPATH += C:/Projects/Cafe5/Service5/utils
INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += C:/Projects/Cafe5/Classes
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include/openssl

LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += -llibcrypto
LIBS += -lwsock32

DEFINES += _APPLICATION_=\\\"Cafe5\\\"
DEFINES += _MODULE_=\\\"Service5_elinashop_dll\\\"

FORMS += \
    ../../../Classes/notificationwidget.ui