cmake_minimum_required(VERSION 3.16)

project(CookingProgress LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets Network Sql WebSockets
)

add_executable(${PROJECT_NAME} WIN32
    main.cpp
    cookingprogresswindow.cpp cookingprogresswindow.h cookingprogresswindow.ui
    version.h
    rc.h
    res.rc
    local.qrc

    ../Cafe5/c5double.cpp
    ../Cafe5/c5systempreference.cpp
    ../Cafe5/calendar.cpp
    ../Cafe5/c5message.cpp
    ../Cafe5/c5utils.cpp
    ../Cafe5/c5dialog.cpp
    ../Cafe5/c5registrysettings.cpp
    ../Cafe5/c5uilanguage.cpp
    ../Cafe5/c5uilanguage.h

    ../Classes/QProgressIndicator.cpp
    ../Classes/appwebsocket.cpp
    ../Classes/c5logtoserverthread.cpp
    ../Classes/c5user.cpp
    ../Classes/c5widget.cpp
    ../Classes/c5threadobject.cpp
    ../Classes/fileversion.cpp
    ../Classes/logwriter.cpp
    ../Classes/c5random.cpp
    ../Classes/c5replacecharacter.cpp

    ../Controls/c5lineedit.cpp
    ../Controls/c5checkbox.cpp
    ../Controls/c5guicontrols.cpp

    ../Forms/c5connectiondialog.cpp
    ../Forms/c5connectiondialog.ui
    ../Forms/dlgsplashscreen.cpp
    ../Forms/dlgsplashscreen.ui

    ../NTable/ndataprovider.cpp
    ../NTable/ninterface.cpp
    ../NTable/nloadingdlg.cpp

    ../Http/httplite.cpp

    ../Service5/worker/struct_workstationitem.cpp
    ../Service5/worker/struct_fiscal_machine.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    .
    ../Cafe5
    ../Classes
    ../Controls
    ../Forms
    ../NTable
    ../Http
    ../Service5/worker
    ../dictionaries
    ../FrontDesk
    C:/Development/OpenSSL-Win64/include
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS
    NETWORKDB
    BORDERLESSDIALOGS
    NOMINMAX
    _DBDRIVER_="QMARIADB"
    _ORGANIZATION_="BreezeDevs"
    _APPLICATION_="CookingProgress"
    _MODULE_="CookingProgress"
    _CRYPT_KEY_="shop2022!!!"
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4834 /utf-8)
endif()

target_link_directories(${PROJECT_NAME} PRIVATE C:/Development/OpenSSL-Win64/lib/VC/x64/MD)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Sql Qt6::WebSockets
    Version.lib Ws2_32.lib libssl.lib libcrypto.lib
)

if(WIN32)
    add_custom_target(version_inc
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File ${CMAKE_CURRENT_SOURCE_DIR}/increase_version.ps1
    )
    add_dependencies(${PROJECT_NAME} version_inc)
endif()
