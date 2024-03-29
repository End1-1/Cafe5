QT += gui network sql printsupport

TEMPLATE = lib
DEFINES += DISCOUNTGENMOBILEP_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../../NewTax/Src/printtaxn.cpp \
    ../../../Classes/QRCodeGenerator.cpp \
    ../../../Classes/c5printing.cpp \
    ../../../Service5Working/raw/rawmessage.cpp \
    ../../socket/sslsocket.cpp \
    ../../utils/configini.cpp \
    ../../utils/database.cpp \
    ../../utils/databaseconnectionmanager.cpp \
    ../../utils/jsonhandler.cpp \
    ../../utils/logwriter.cpp \
    ../../utils/networktable.cpp \
    c5translator.cpp \
    login.cpp \
    ops.cpp \
    waiterclienthandler.cpp \
    waiterclientp.cpp \
    waiterconnection.cpp

HEADERS += \
    ../../../../NewTax/Src/printtaxn.h \
    ../../../Classes/QRCodeGenerator.h \
    ../../../Classes/c5printing.h \
    ../../../Service5Working/raw/rawmessage.h \
    ../../socket/sslsocket.h \
    ../../utils/configini.h \
    ../../utils/database.h \
    ../../utils/databaseconnectionmanager.h \
    ../../utils/jsonhandler.h \
    ../../utils/logwriter.h \
    ../../utils/networktable.h \
    c5translator.h \
    login.h \
    ops.h \
    waiterclienthandler.h \
    waiterclientp.h \
    waiterconnection.h

DEFINES += _APPLICATION_=\\\"Breeze\\\"
DEFINES += _MODULE_=\\\"Service5_waiterclientp_dll\\\"

INCLUDEPATH += ../../raw
INCLUDEPATH += ../../socket
INCLUDEPATH += ../../utils
INCLUDEPATH += ../../../Classes
INCLUDEPATH += ../../../Service5Working/raw

INCLUDEPATH += C:/Projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include
INCLUDEPATH += C:/Soft/OpenSSL-Win64/include/openssl
LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += -llibcrypto
LIBS += -lwsock32

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../../res/waiterclientp.qrc
