cmake_minimum_required(VERSION 3.16)

project(Shop_net LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)       # Автоматическая генерация ui_*.h
set(CMAKE_AUTOMOC ON)       # Автоматическая генерация moc_*.cpp
set(CMAKE_AUTORCC ON)       # Автоматическая генерация из .qrc


# -------------------------
# Qt
# -------------------------

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    PrintSupport
    Network
    Sql
    Multimedia
    WebSockets
)

# -------------------------
# Sources
# -------------------------


set(RESOURCES
    ../resources/res.qrc
    local.qrc
)

set(WIN_RC
    res.rc
)


# -------------------------
# Executable
# -------------------------

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${RESOURCES}
    ${WIN_RC}
    ../Cafe5/c5double.cpp
    ../Cafe5/c5systempreference.cpp
    ../Cafe5/calendar.cpp
    ../Classes/QProgressIndicator.cpp
    ../Classes/QRCodeGenerator.cpp
    ../Classes/appwebsocket.cpp
    ../Classes/c5airlog.cpp
    ../Classes/c5combodelegate.cpp
    ../Classes/c5datedelegate.cpp
    ../Classes/c5jsondb.cpp
    ../Classes/c5logtoserverthread.cpp
    ../Classes/c5menu.cpp
    ../Classes/c5permissions.cpp
    ../Classes/c5printing.cpp
    ../Classes/c5printjson.cpp
    ../Classes/c5printrecipta4.cpp
    ../Classes/c5random.cpp
    ../Classes/c5replacecharacter.cpp
    ../Classes/c5reporttemplatedriver.cpp
    ../Classes/c5servername.cpp
    ../Classes/c5shoporder.cpp
    ../Classes/c5storedraftwriter.cpp
    ../Classes/c5tablemodel.cpp
    ../Classes/c5tablerec.cpp
    ../Classes/c5tableview.cpp
    ../Classes/c5textdelegate.cpp
    ../Classes/c5threadobject.cpp
    ../Classes/c5user.cpp
    ../Classes/c5widget.cpp
    ../Classes/c5witerconf.cpp
    ../Classes/calculator.cpp

    ../Classes/fileversion.cpp
    ../Classes/idram.cpp
    ../Classes/logwriter.cpp
    ../Classes/notificationwidget.cpp
    ../Classes/outputofheader.cpp
    ../Classes/proxytablewidgetdatabase.cpp
    ../Configs/breezeconfig.cpp
    ../Configs/configs.cpp

    ../Controls/c5graphicsview.cpp
    ../Controls/c5guicontrols.cpp
    ../DbData/datadriver.cpp
    ../DbData/dbdata.cpp
    ../DbData/dbgoods.cpp
    ../DbData/dbgoodsgroup.cpp
    ../DbData/dbstore.cpp
    ../DbData/dbstoredoctype.cpp
    ../DbData/dbunit.cpp
    ../Forms/c5connectiondialog.cpp
    ../Forms/dlgpaymentchoose.cpp
    ../Forms/dlgreservgoods.cpp
    ../Forms/dlgsplashscreen.cpp
    ../NTable/ndataprovider.cpp
    ../NTable/ninterface.cpp
    ../NTable/nloadingdlg.cpp
    ../Printing/printreceipt.cpp
    ../Printing/printreceiptgroup.cpp
    ../Printing/selectprinters.cpp
    ../StructModel/c5structtableview.cpp
    ../TableRecord/aheader.cpp
    ../TableRecord/bclientdebts.cpp
    ../TableRecord/bhistory.cpp
    ../TableRecord/c5dbrecord.cpp
    ../TableRecord/cpartners.cpp
    ../TableRecord/ecash.cpp
    ../TableRecord/odraftsale.cpp
    ../TableRecord/ogoods.cpp
    ../TableRecord/oheader.cpp
    ../TableRecord/otax.cpp
    c5shopdialog.cpp
    c5tempsale.cpp
    cashcollection.cpp
    dlgcashout.cpp
    dlgdate.cpp
    dlggetidname.cpp
    dlggetvalue.cpp
    dlggiftcardsale.cpp
    dlggoodslist.cpp
    dlgmovement.cpp
    dlgpin.cpp
    dlgregistercard.cpp
    dlgreturnitem.cpp
    dlgshowcolumns.cpp
    goodsreturnreason.cpp
        main.cpp
    sales.cpp
    searchitems.cpp
    selectstaff.cpp
    storeinput.cpp
    threadworker.cpp
    userphoto.cpp
    viewinputitem.cpp
    vieworder.cpp
    wcustomerdisplay.cpp
        working.cpp
    worder.cpp
    ../Cafe5/c5config.cpp
    ../Cafe5/c5database.cpp
    ../Cafe5/c5message.cpp
    ../Cafe5/c5utils.cpp
    ../Cafe5/c5dialog.cpp
    ../Controls/c5lineedit.cpp
    ../Controls/c5combobox.cpp
    ../Controls/c5dateedit.cpp
    ../Classes/c5cache.cpp
    ../../desktop.newTax/Src/printtaxn.cpp
    dqty.cpp
    ../Controls/c5checkbox.cpp
    ../Cafe5/c5double.h
    ../Cafe5/c5systempreference.h
    ../Cafe5/calendar.h
    ../Classes/QProgressIndicator.h
    ../Classes/QRCodeGenerator.h
    ../Classes/appwebsocket.h
    ../Classes/barcode5.h
    ../Classes/c5airlog.h
    ../Classes/c5combodelegate.h
    ../Classes/c5datedelegate.h
    ../Classes/c5jsondb.h
    ../Classes/c5logtoserverthread.h
    ../Classes/c5menu.h
    ../Classes/c5permissions.h
    ../Classes/c5printing.h
    ../Classes/c5printjson.h
    ../Classes/c5printrecipta4.h
    ../Classes/c5random.h
    ../Classes/c5replacecharacter.h
    ../Classes/c5reporttemplatedriver.h
    ../Classes/c5servername.h
    ../Classes/c5shoporder.h
    ../Classes/c5storedraftwriter.h
    ../Classes/c5tablemodel.h
    ../Classes/c5tablerec.h
    ../Classes/c5tableview.h
    ../Classes/c5textdelegate.h
    ../Classes/c5threadobject.h
    ../Classes/c5user.h
    ../Classes/c5widget.h
    ../Classes/c5witerconf.h
    ../Classes/calculator.h
    ../Classes/chatmessage.h

    ../Classes/fileversion.h
    ../Classes/goodsreserve.h
    ../Classes/idram.h
    ../Classes/logwriter.h
    ../Classes/notificationwidget.h
    ../Classes/outputofheader.h
    ../Classes/proxytablewidgetdatabase.h
    ../Configs/breezeconfig.h
    ../Configs/configs.h

    ../Controls/c5graphicsview.h
    ../Controls/c5guicontrols.h
    ../DbData/datadriver.h
    ../DbData/dbdata.h
    ../DbData/dbgoods.h
    ../DbData/dbgoodsgroup.h
    ../DbData/dbstore.h
    ../DbData/dbstoredoctype.h
    ../DbData/dbunit.h
    ../Forms/c5connectiondialog.h
    ../Forms/dlgpaymentchoose.h
    ../Forms/dlgreservgoods.h
    ../Forms/dlgsplashscreen.h
    ../NTable/ndataprovider.h
    ../NTable/ninterface.h
    ../NTable/nloadingdlg.h
    ../Printing/printreceipt.h
    ../Printing/printreceiptgroup.h
    ../Printing/selectprinters.h
    ../Service5/worker/struct_storage_item.h
    ../Service5/worker/c5jsonparser.h
    ../Service5/worker/c5structtraits.h
    ../Service5/worker/struct_goods_item.h
    ../Service5/worker/struct_partner.h
    ../Service5/worker/struct_storage_item.h
    ../StructModel/c5structmodel.h
    ../StructModel/c5structtableview.h
    ../TableRecord/aheader.h
    ../TableRecord/bclientdebts.h
    ../TableRecord/bhistory.h
    ../TableRecord/c5dbrecord.h
    ../TableRecord/cpartners.h
    ../TableRecord/ecash.h
    ../TableRecord/odraftsale.h
    ../TableRecord/ogoods.h
    ../TableRecord/oheader.h
    ../TableRecord/otax.h
    c5shopdialog.h
    c5tempsale.h
    cashcollection.h
    dlgcashout.h
    dlgdate.h
    dlggetidname.h
    dlggetvalue.h
    dlggiftcardsale.h
    dlggoodslist.h
    dlgmovement.h
    dlgpin.h
    dlgregistercard.h
    dlgreturnitem.h
    dlgshowcolumns.h
    goodscols.h
    goodsreturnreason.h
    rc.h
    sales.h
    searchitems.h
    selectstaff.h
    storeinput.h
    threadworker.h
    userphoto.h
    version.h
    viewinputitem.h
    vieworder.h
    wcustomerdisplay.h
        working.h
    worder.h
    ../Cafe5/c5config.h
    ../Cafe5/c5database.h
    ../Cafe5/c5message.h
    ../Cafe5/c5utils.h
    ../Cafe5/c5dialog.h
    ../Controls/c5lineedit.h
    ../Controls/c5combobox.h
    ../Controls/c5dateedit.h
    ../Classes/c5cache.h
    ../../desktop.newtax/Src/printtaxn.h
    dqty.h
    ../Controls/c5checkbox.h
    ../Cafe5/calendar.ui
    ../Classes/calculator.ui
    ../Classes/notificationwidget.ui
    ../Controls/c5tablewithtotal.ui
    ../Forms/c5connectiondialog.ui
    ../Forms/dlgpaymentchoose.ui
    ../Forms/dlgreservgoods.ui
    ../Forms/dlgsplashscreen.ui
    ../NTable/nloadingdlg.ui
    ../Printing/selectprinters.ui
    ../StructModel/c5structtableview.ui
    c5tempsale.ui
    cashcollection.ui
    dlgcashout.ui
    dlgdate.ui
    dlggetidname.ui
    dlggetvalue.ui
    dlggiftcardsale.ui
    dlggoodslist.ui
    dlgmovement.ui
    dlgpin.ui
    dlgregistercard.ui
    dlgreturnitem.ui
    dlgshowcolumns.ui
    goodsreturnreason.ui
    sales.ui
    searchitems.ui
    selectstaff.ui
    storeinput.ui
    userphoto.ui
    viewinputitem.ui
    vieworder.ui
    wcustomerdisplay.ui
        working.ui
    worder.ui
    ../Cafe5/c5message.ui
    dqty.ui
    ../Classes/c5codenameselectorfunctions.h
    ../Classes/fileversion.cpp ../Classes/fileversion.h
    ../Controls/c5cleartablewidget.cpp ../Controls/c5cleartablewidget.h


)

# -------------------------
# Qt automoc/uic/rcc
# -------------------------

set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    WIN32_EXECUTABLE TRUE
)

# -------------------------
# Include paths
# -------------------------

target_include_directories(${PROJECT_NAME} PRIVATE
    ./
    ../Cafe5
    ../DBData
    ../TableRecord
    ../NTable
    ../Classes
    ../Cache
    ../Configs
    ../Controls
    ../DbData
    ../Forms
    ../Configs
    ../Service
    ../Service5/worker
    ../StructModel
    ../Printing
    ../RESOURCES
    C:/Development/projects/desktop.newtax/Src
    C:/Development/OpenSSL-Win64/include
)



# -------------------------
# Compile definitions
# -------------------------

target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS
    NETWORKDB
    SHOP
    BORDERLESSDIALOGS

    _DBDRIVER_="QMARIADB"
    _ORGANIZATION_="BreezeDevs"
    _APPLICATION_="Shop"
    _MODULE_="Shop"
    _CRYPT_KEY_="shop2022!!!"
)

# -------------------------
# Link Qt
# -------------------------

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::WebSockets
)

# -------------------------
# Link OpenSSL + Windows libs
# -------------------------

# Указываем путь к OpenSSL (если find_package не используем)
target_include_directories(${PROJECT_NAME} PRIVATE
    C:/Development/OpenSSL-Win64/include
    C:/Development/OpenSSL-Win64/include/openssl
)

# Линковка библиотек
target_link_directories(${PROJECT_NAME} PRIVATE
    C:/Development/OpenSSL-Win64/lib/VC/x64/MD
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Version.lib
    Ws2_32.lib
    libssl.lib
    libcrypto.lib
    Gdi32.lib
    User32.lib
)



# Отключаем CPack внутри QXlsx
set(CPACK_GENERATOR "" CACHE STRING "Disable CPack in QXlsx" FORCE)
set(CPACK_RESOURCE_FILE_README "" CACHE STRING "" FORCE)

# Подключаем QXlsx
add_subdirectory(C:/Development/projects/qxlsx qxlsx_build)

# Линкуем к твоему проекту
target_link_libraries(${PROJECT_NAME} PRIVATE QXlsx)

# -------------------------
# Icon
# -------------------------

set_target_properties(${PROJECT_NAME} PROPERTIES
    RESOURCE bag.ico
)

# -------------------------
# Version auto increment (PowerShell)
# -------------------------

if (WIN32)

    add_custom_target(version_inc
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass
        -File ${CMAKE_CURRENT_SOURCE_DIR}/increase_version.ps1
    )

    add_dependencies(${PROJECT_NAME} version_inc)

endif()
