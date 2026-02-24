cmake_minimum_required(VERSION 3.16)

project(Smart LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем Unity Build глобально или для конкретного таргета (сделаем для Smart)
set(CMAKE_UNITY_BUILD OFF)

# Отключаем strictStrings как в qmake
if(MSVC)
    add_compile_options(/Zc:strictStrings-)
    # win32-msvc*: QMAKE_CXXFLAGS += /WX (Treat warnings as errors)
    add_compile_options(/WX)
else()
    # win32-g++: QMAKE_CXXFLAGS += -Werror
    add_compile_options(-Werror)
endif()

# Поиск пакетов Qt
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Network
    PrintSupport
    Sql
    Multimedia
    WebSockets
    MultimediaWidgets
    Widgets
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Конфигурационные дефайны
add_definitions(
    -DWIN32_LEAN_AND_MEAN
    -DQT_DEPRECATED_WARNINGS
    -DWAITER
    -DNETWORKDB
    -DSMART
    -DBORDERLESSDIALOGS
    -D_DBDRIVER_="QMARIADB"
    -D_ORGANIZATION_="Jazzve"
    -D_APPLICATION_="Cafe5"
    -D_MODULE_="SMART"
)

# Пути поиска (Include paths)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    "c:/Development/projects/cafe5/Cafe5"
    "c:/Development/projects/cafe5/Classes"
    "c:/Development/projects/cafe5/Controls"
    "c:/Development/projects/cafe5/Service"
    "c:/Development/projects/cafe5/TableRecord"
    "c:/Development/projects/cafe5/service5working/utils"
    "c:/Development/projects/cafe5/DbData"
    "c:/Development/projects/cafe5/Forms"
    "c:/Development/projects/cafe5/NTable"
    "c:/Development/projects/xlsx/src"
    ../Service5/worker
    C:/Development/projects/desktop.newtax/Src
    C:/Development/OpenSSL-Win64/include
)



# Список заголовочных файлов (для порядка в IDE)
set(HEADERS
    ../Classes/QProgressIndicator.h
    ../Classes/amkbd.h
    ../Classes/appwebsocket.h
    ../Classes/c5logtoserverthread.h
    ../Classes/scopeguarde.h
    ../Forms/c5connectiondialog.h
    ../Forms/dlgpin.h
    ../NTable/ndataprovider.h
    ../NTable/ninterface.h
    ../NTable/nloadingdlg.h
    c5waiterdialog.h
    dlgservicevalues.h
    dlgvisit.h
    registercustomer.h
    version.h
    ../Classes/c5servername.h
    ../Classes/dict_dish_state.h
    ../Forms/dlgsplashscreen.h
    ../Cafe5/c5double.h
    ../Cafe5/c5systempreference.h
    ../Cafe5/calendar.h

    ../Classes/barcode5.h

    ../Classes/c5cache.h
    ../Classes/c5combodelegate.h
    ../Classes/c5datedelegate.h
    ../Classes/c5jsondb.h
    ../Classes/c5menu.h
    ../Classes/c5permissions.h
    ../Classes/c5printing.h
    ../Classes/c5printjson.h
    ../Classes/c5random.h
    ../Classes/c5storedraftwriter.h
    ../Classes/c5tablemodel.h
    ../Classes/c5tablerec.h
    ../Classes/c5tableview.h
    ../Classes/c5textdelegate.h
    ../Classes/c5threadobject.h
    ../Classes/c5waiterorderdoc.h
    ../Classes/c5widget.h
    ../Classes/c5witerconf.h
    ../Classes/calculator.h
    ../Classes/dlgpassword.h
    ../Classes/fileversion.h
    ../Classes/idram.h
    ../Classes/notificationwidget.h
    ../Classes/rkeyboard.h
    ../Controls/c5checkbox.h
    ../Controls/c5combobox.h
    ../Controls/c5dateedit.h
    ../Controls/c5graphicsview.h
    ../Controls/c5guicontrols.h
    ../Forms/change.h
    ../Forms/customerinfo.h
    ../Forms/dlgcashinout.h
    ../Forms/dlgcashop.h
    ../Forms/dlglistofdishcomments.h
    ../Forms/dlgqty.h
    ../Forms/touchdlgphonenumber.h
    ../Forms/touchentertaxreceiptnumber.h
    ../Forms/touchselecttaxreport.h

    ../TableRecord/aheader.h
    ../TableRecord/bclientdebts.h
    ../TableRecord/bhistory.h
    ../TableRecord/c5dbrecord.h
    ../TableRecord/cpartners.h
    ../TableRecord/ecash.h
    ../TableRecord/ogoods.h
    ../TableRecord/oheader.h
    dish.h
    dishpackage.h
    dlgmemoryread.h
    dlgsmartreports.h
    menudialog.h
    menudish.h
    rc.h
    sessionorders.h
    sessionshistory.h
    supplier.h
    wcustomerdisplay.h
    workspace.h
    ../Cafe5/c5config.h
    ../Cafe5/c5database.h
    ../Cafe5/c5dialog.h
    ../Cafe5/c5message.h
    ../Cafe5/c5utils.h
    ../Controls/c5lineedit.h
    ../Classes/c5user.h
)

# Список исходников
set(SOURCES
    main.cpp
    ../Classes/QProgressIndicator.cpp
    ../Classes/amkbd.cpp
    ../Classes/appwebsocket.cpp
    ../Classes/scopeguarde.cpp
    ../Forms/c5connectiondialog.cpp
    ../Forms/dlgpin.cpp
    ../NTable/ndataprovider.cpp
    ../NTable/ninterface.cpp
    ../NTable/nloadingdlg.cpp

    c5waiterdialog.cpp
    dlgservicevalues.cpp
    dlgvisit.cpp
    registercustomer.cpp
    ../Classes/c5servername.cpp
    ../Forms/dlgsplashscreen.cpp
    ../Cafe5/c5double.cpp
    ../Cafe5/c5systempreference.cpp
    ../Cafe5/calendar.cpp


    ../Classes/c5cache.cpp
    ../Classes/c5combodelegate.cpp
    ../Classes/c5datedelegate.cpp
    ../Classes/c5jsondb.cpp
    ../Classes/c5logtoserverthread.cpp
    ../Classes/c5menu.cpp
    ../Classes/c5permissions.cpp
    ../Classes/c5printing.cpp
    ../Classes/c5printjson.cpp
    ../Classes/c5random.cpp
    ../Classes/c5storedraftwriter.cpp
    ../Classes/c5tablemodel.cpp
    ../Classes/c5tablerec.cpp
    ../Classes/c5tableview.cpp
    ../Classes/c5textdelegate.cpp
    ../Classes/c5threadobject.cpp
    ../Classes/c5waiterorderdoc.cpp
    ../Classes/c5widget.cpp
    ../Classes/c5witerconf.cpp
    ../Classes/calculator.cpp
    ../Classes/dlgpassword.cpp
    ../Classes/fileversion.cpp
    ../Classes/idram.cpp
    ../Classes/notificationwidget.cpp
    ../Classes/rkeyboard.cpp
    ../Controls/c5checkbox.cpp
    ../Controls/c5combobox.cpp
    ../Controls/c5dateedit.cpp
    ../Controls/c5graphicsview.cpp
    ../Controls/c5guicontrols.cpp
    ../Forms/change.cpp
    ../Forms/customerinfo.cpp
    ../Forms/dlgcashinout.cpp
    ../Forms/dlgcashop.cpp
    ../Forms/dlglistofdishcomments.cpp
    ../Forms/dlgqty.cpp
    ../Forms/touchdlgphonenumber.cpp
    ../Forms/touchentertaxreceiptnumber.cpp
    ../Forms/touchselecttaxreport.cpp
    ../TableRecord/aheader.cpp
    ../TableRecord/bclientdebts.cpp
    ../TableRecord/bhistory.cpp
    ../TableRecord/c5dbrecord.cpp
    ../TableRecord/cpartners.cpp
    ../TableRecord/ecash.cpp
    ../TableRecord/ogoods.cpp
    ../TableRecord/oheader.cpp
    dishpackage.cpp
    dlgmemoryread.cpp
    dlgsmartreports.cpp
    menudialog.cpp
    menudish.cpp
    sessionorders.cpp
    sessionshistory.cpp
    supplier.cpp
    wcustomerdisplay.cpp
    workspace.cpp
    ../Cafe5/c5config.cpp
    ../Cafe5/c5database.cpp
    ../Cafe5/c5dialog.cpp
    ../Cafe5/c5message.cpp
    ../Cafe5/c5utils.cpp
    ../Controls/c5lineedit.cpp
    ../Classes/c5user.cpp
)

# UI формы
set(FORMS
    ../Forms/c5connectiondialog.ui
    ../Forms/dlgpin.ui
    ../NTable/nloadingdlg.ui
    dlgservicevalues.ui
    dlgvisit.ui
    registercustomer.ui
    ../Forms/dlgsplashscreen.ui
    ../Cafe5/calendar.ui
    ../Classes/calculator.ui
    ../Classes/dlgpassword.ui
    ../Classes/notificationwidget.ui
    ../Classes/rkeyboard.ui
    ../Forms/change.ui
    ../Forms/customerinfo.ui
    ../Forms/dlgcashinout.ui
    ../Forms/dlgcashop.ui
    ../Forms/dlglistofdishcomments.ui
    ../Forms/dlgqty.ui
    ../Forms/touchdlgphonenumber.ui
    ../Forms/touchentertaxreceiptnumber.ui
    ../Forms/touchselecttaxreport.ui
    dlgmemoryread.ui
    dlgsmartreports.ui
    menudialog.ui
    menudish.ui
    sessionorders.ui
    sessionshistory.ui
    supplier.ui
    wcustomerdisplay.ui
    workspace.ui
    ../Cafe5/c5message.ui
)

# Ресурсы
set(RESOURCES
    ../resources/resources.qrc
    transres.qrc
)

# Исполняемый файл
if(WIN32)
    set(EXE_TYPE WIN32)
    set(RC_SOURCES res.rc)
endif()

add_executable(Smart ${EXE_TYPE}
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
    ${RC_SOURCES}
    ../Classes/format_date.h
    ../../desktop.newtax/Src/printtaxn.cpp ../../desktop.newtax/Src/printtaxn.h
    ../Classes/logwriter.cpp ../Classes/logwriter.h
    ../Service5/worker/c5jsonparser.h ../Service5/worker/c5structtraits.h ../Service5/worker/socketstruct.cpp ../Service5/worker/socketstruct.h ../Service5/worker/struct_dish.h ../Service5/worker/struct_goods_group.h ../Service5/worker/struct_goods_item.h ../Service5/worker/struct_hall.h ../Service5/worker/struct_parent.h ../Service5/worker/struct_partner.h ../Service5/worker/struct_storage_item.h ../Service5/worker/struct_table.h ../Service5/worker/struct_waiter_dish.h ../Service5/worker/struct_waiter_order.h ../Service5/worker/struct_workstationitem.cpp ../Service5/worker/struct_workstationitem.h
)

# Настройки Unity Build для конкретной цели
set_target_properties(Smart PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 0 # 0 — CMake сам определит оптимальный размер пачек
)

target_link_directories(${PROJECT_NAME} PRIVATE
    C:/Development/OpenSSL-Win64/lib/VC/x64/MD
)

# Линковка библиотек
target_link_libraries(Smart PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::PrintSupport
    Qt6::Sql
    Qt6::Multimedia
    Qt6::WebSockets
    Qt6::MultimediaWidgets
    Qt6::Widgets
    Version.lib Ws2_32.lib libssl.lib libcrypto.lib Gdi32.lib User32.lib
)

# Обработка версии (PowerShell скрипт)
if(WIN32)
    add_custom_command(
        TARGET Smart
        PRE_BUILD
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/increase_version.ps1"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Increasing version via PowerShell..."
    )
endif()
