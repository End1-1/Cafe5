cmake_minimum_required(VERSION 3.16)

project(Shop_net LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем Unity Build (выключен по умолчанию для стабильности lupdate)
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

# ЧИСТЫЙ список исходников без дубликатов
set(APP_SOURCES
    # Внешние папки
    ../Cafe5/c5double.cpp ../Cafe5/c5systempreference.cpp ../Cafe5/calendar.cpp ../Cafe5/c5message.cpp
    ../Cafe5/c5utils.cpp ../Cafe5/c5dialog.cpp
    ../Classes/QProgressIndicator.cpp ../Classes/QRCodeGenerator.cpp ../Classes/appwebsocket.cpp

    ../Classes/c5logtoserverthread.cpp ../Classes/c5menu.cpp ../Classes/c5permissions.cpp
    ../Classes/c5printing.cpp ../Classes/c5printjson.cpp ../Classes/c5printrecipta4.cpp
    ../Classes/c5random.cpp ../Classes/c5replacecharacter.cpp
 ../Classes/c5threadobject.cpp ../Classes/c5user.cpp
    ../Classes/c5widget.cpp ../Classes/c5witerconf.cpp ../Classes/calculator.cpp
    ../Classes/fileversion.cpp ../Classes/idram.cpp ../Classes/logwriter.cpp
    ../Classes/notificationwidget.cpp


    ../Controls/c5graphicsview.cpp ../Controls/c5guicontrols.cpp ../Controls/c5lineedit.cpp
  ../Controls/c5dateedit.cpp ../Controls/c5checkbox.cpp
    ../Controls/c5cleartablewidget.cpp
    ../Forms/c5connectiondialog.cpp ../Forms/dlgreservgoods.cpp ../Forms/dlgsplashscreen.cpp ../Forms/previewdialog.cpp
    ../NTable/ndataprovider.cpp ../NTable/ninterface.cpp ../NTable/nloadingdlg.cpp
    ../Printing/selectprinters.cpp
    ../StructModel/c5structtableview.cpp
    ../Service5/worker/struct_workstationitem.cpp ../Service5/worker/struct_fiscal_machine.cpp
    ../../desktop.newTax/Src/printtaxn.cpp
    ../Service5/worker/struct_waiter_order.h

    ../Http/httplite.h
    ../Http/httplite.cpp


    # Файлы в корне проекта (по одному разу!)
    c5shopdialog.cpp dlgcashout.cpp dlgdate.cpp
    dlggetvalue.cpp dlggiftcardsale.cpp dlggoodslist.cpp
    dlgpin.cpp dlgregistercard.cpp dlgshopcustomer.cpp dlgshowcolumns.cpp
    dqty.cpp goodsreturnreason.cpp main.cpp sales.cpp searchitems.cpp selectstaff.cpp
    threadworker.cpp userphoto.cpp  vieworder.cpp
    wcustomerdisplay.cpp working.cpp worder.cpp
    dlgpaymentchoose.cpp dlgpaymentchoose.h dlgpaymentchoose.ui
    printreceiptgroup.cpp printreceiptgroup.h
    version.h

    # Ресурсы
    ${RESOURCES} ${WIN_RC}
)

# -------------------------
# Executable
# -------------------------
add_executable(${PROJECT_NAME} ${APP_SOURCES}
    ../dictionaries/dict_payment_type.cpp ../dictionaries/dict_payment_type.h)

# -------------------------
# Настройки Unity Build и исключения
# -------------------------
set_source_files_properties(
    "../Classes/QRCodeGenerator.cpp"
    "../../desktop.newTax/Src/printtaxn.cpp"
    "../Cafe5/c5utils.cpp"
    "sales.cpp"
    PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# -------------------------
# Include paths
# -------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    . ../Cafe5  ../TableRecord ../NTable ../Classes ../Cache
    ../Controls ../Forms ../Service ../Service5/worker ../StructModel
    ../Printing ../RESOURCES ../Http
    C:/Development/projects/desktop.newtax/Src
    C:/Development/OpenSSL-Win64/include
)

# -------------------------
# Compile Definitions & Options
# -------------------------
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS NETWORKDB SHOP BORDERLESSDIALOGS NOMINMAX
    _DBDRIVER_="QMARIADB" _ORGANIZATION_="BreezeDevs" _APPLICATION_="Shop"
    _MODULE_="Shop" _CRYPT_KEY_="shop2022!!!"
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4834 /utf-8)
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
# QXlsx
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

# Просто говорим CMake, где лежит готовый TS файл
set(TS_V_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Shop.ts")

# Оставляем ТОЛЬКО lrelease.
# lupdate (обновление строк) будешь делать только когда сам захочешь
# через контекстное меню или команду выше.
qt_add_lrelease(${PROJECT_NAME}
    TS_FILES ${TS_V_FILE}
    QM_FILES_OUTPUT_VARIABLE QM_FILES
)

target_sources(${PROJECT_NAME} PRIVATE ${QM_FILES})
