cmake_minimum_required(VERSION 3.16)

project(Shop_net LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем Unity Build глобально
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 16)

# Автоматизация Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# -------------------------
# Qt6
# -------------------------
find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets PrintSupport Network Sql Multimedia Pdf PdfWidgets WebSockets LinguistTools
)

set(TS_FILES "Shop.ts")

# -------------------------
# Sources & Resources
# -------------------------
set(RESOURCES ../resources/res.qrc local.qrc)
set(WIN_RC res.rc)

# Список исходников (я немного почистил дубликаты расширений)
set(APP_SOURCES
    ../Cafe5/c5double.cpp ../Cafe5/c5systempreference.cpp ../Cafe5/calendar.cpp
    ../Classes/QProgressIndicator.cpp ../Classes/QRCodeGenerator.cpp ../Classes/appwebsocket.cpp
    ../Classes/c5airlog.cpp ../Classes/c5combodelegate.cpp ../Classes/c5datedelegate.cpp
    ../Classes/c5jsondb.cpp ../Classes/c5logtoserverthread.cpp ../Classes/c5menu.cpp
    ../Classes/c5permissions.cpp ../Classes/c5printing.cpp ../Classes/c5printjson.cpp
    ../Classes/c5printrecipta4.cpp ../Classes/c5random.cpp ../Classes/c5replacecharacter.cpp
    ../Classes/c5reporttemplatedriver.cpp ../Classes/c5servername.cpp ../Classes/c5shoporder.cpp
    ../Classes/c5storedraftwriter.cpp ../Classes/c5tablemodel.cpp ../Classes/c5tablerec.cpp
    ../Classes/c5tableview.cpp ../Classes/c5textdelegate.cpp ../Classes/c5threadobject.cpp
    ../Classes/c5user.cpp ../Classes/c5widget.cpp ../Classes/c5witerconf.cpp ../Classes/calculator.cpp
    ../Classes/fileversion.cpp ../Classes/idram.cpp ../Classes/logwriter.cpp
    ../Classes/notificationwidget.cpp ../Classes/outputofheader.cpp ../Classes/proxytablewidgetdatabase.cpp
    ../Configs/breezeconfig.cpp ../Configs/configs.cpp ../Controls/c5graphicsview.cpp
    ../Controls/c5guicontrols.cpp ../DbData/datadriver.cpp ../DbData/dbdata.cpp
    ../DbData/dbgoods.cpp ../DbData/dbgoodsgroup.cpp ../DbData/dbstore.cpp
    ../DbData/dbstoredoctype.cpp ../DbData/dbunit.cpp ../Forms/c5connectiondialog.cpp
    ../Forms/dlgpaymentchoose.cpp ../Forms/dlgreservgoods.cpp ../Forms/dlgsplashscreen.cpp
    ../NTable/ndataprovider.cpp ../NTable/ninterface.cpp ../NTable/nloadingdlg.cpp
    ../Printing/printreceipt.cpp ../Printing/printreceiptgroup.cpp ../Printing/selectprinters.cpp
    ../StructModel/c5structtableview.cpp ../TableRecord/aheader.cpp ../TableRecord/bclientdebts.cpp
    ../TableRecord/bhistory.cpp ../TableRecord/c5dbrecord.cpp ../TableRecord/cpartners.cpp
    ../TableRecord/ecash.cpp ../TableRecord/odraftsale.cpp ../TableRecord/ogoods.cpp
    ../TableRecord/oheader.cpp ../TableRecord/otax.cpp
    c5shopdialog.cpp c5tempsale.cpp cashcollection.cpp dlgcashout.cpp dlgdate.cpp
    dlggetidname.cpp dlggetvalue.cpp dlggiftcardsale.cpp dlggoodslist.cpp dlgmovement.cpp
    dlgpin.cpp dlgregistercard.cpp dlgreturnitem.cpp dlgshowcolumns.cpp goodsreturnreason.cpp
    main.cpp sales.cpp searchitems.cpp selectstaff.cpp storeinput.cpp threadworker.cpp
    userphoto.cpp viewinputitem.cpp vieworder.cpp wcustomerdisplay.cpp working.cpp worder.cpp
    ../Cafe5/c5config.cpp ../Cafe5/c5database.cpp ../Cafe5/c5message.cpp ../Cafe5/c5utils.cpp
    ../Cafe5/c5dialog.cpp ../Controls/c5lineedit.cpp ../Controls/c5combobox.cpp
    ../Controls/c5dateedit.cpp ../Classes/c5cache.cpp ../../desktop.newTax/Src/printtaxn.cpp
    dqty.cpp ../Controls/c5checkbox.cpp ../Controls/c5cleartablewidget.cpp
    dlgshopcustomer.cpp ../Forms/previewdialog.cpp ../Service5/worker/struct_workstationitem.cpp
    # UI и заголовочные файлы (AUTOMOC/AUTOUIC их подхватят)
    ${RESOURCES} ${WIN_RC}
)

# -------------------------
# Executable
# -------------------------
add_executable(${PROJECT_NAME} ${APP_SOURCES})

# -------------------------
# Настройки Unity Build и исключения
# -------------------------
# Исключаем файлы, которые «ломают» сборку при склейке
set_source_files_properties(
    "../Classes/QRCodeGenerator.cpp"
    "../../desktop.newTax/Src/printtaxn.cpp"
    "../Cafe5/c5utils.cpp"
    "sales.cpp" # Пример, если форма будет ругаться на incomplete type
    PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# -------------------------
# Include paths
# -------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ./ ../Cafe5 ../DBData ../TableRecord ../NTable ../Classes ../Cache ../Configs
    ../Controls ../DbData ../Forms ../Service ../Service5/worker ../StructModel
    ../Printing ../RESOURCES
    C:/Development/projects/desktop.newtax/Src
    C:/Development/OpenSSL-Win64/include
)

# -------------------------
# Compile Definitions & Options
# -------------------------
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS NETWORKDB SHOP BORDERLESSDIALOGS NOMINMAX # NOMINMAX лечит ошибки с std::min
    _DBDRIVER_="QMARIADB" _ORGANIZATION_="BreezeDevs" _APPLICATION_="Shop"
    _MODULE_="Shop" _CRYPT_KEY_="shop2022!!!"
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4834 /utf-8) # Глушим варнинги [[nodiscard]]
endif()

# -------------------------
# Link Libraries
# -------------------------
target_link_directories(${PROJECT_NAME} PRIVATE C:/Development/OpenSSL-Win64/lib/VC/x64/MD)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::PrintSupport Qt6::Network Qt6::Sql
    Qt6::Multimedia Qt6::WebSockets Qt6::Pdf Qt6::PdfWidgets
    Version.lib Ws2_32.lib libssl.lib libcrypto.lib Gdi32.lib User32.lib
)

# -------------------------
# QXlsx (внешняя либа)
# -------------------------
set(CPACK_GENERATOR "" CACHE STRING "Disable CPack in QXlsx" FORCE)
add_subdirectory(C:/Development/projects/qxlsx qxlsx_build)
target_link_libraries(${PROJECT_NAME} PRIVATE QXlsx)

# -------------------------
# Version & Translations
# -------------------------
if (WIN32)
    add_custom_target(version_inc
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File ${CMAKE_CURRENT_SOURCE_DIR}/increase_version.ps1
    )
    add_dependencies(${PROJECT_NAME} version_inc)
endif()

target_sources(${PROJECT_NAME} PRIVATE ${TS_FILES})
qt_add_lupdate(${PROJECT_NAME} TS_FILES ${TS_FILES} SOURCES ${APP_SOURCES})
qt_add_lrelease(${PROJECT_NAME} TS_FILES ${TS_FILES} NO_TARGET_DEPENDENCY)
