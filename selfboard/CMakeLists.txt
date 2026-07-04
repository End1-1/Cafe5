cmake_minimum_required(VERSION 3.21)

project(SelfBoard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network LinguistTools)

include_directories(
    ../NTable
    ../Classes
    ../Cafe5
)

set(SOURCES
    main.cpp
    dlgstart.cpp
    dlgserversettings.cpp
    serverconfig.cpp
    appsettings.cpp
    selfboardsettings.cpp
    selfboardlanguage.cpp
    selfboardbottomchrome.cpp
    cartflyanimation.cpp
    dlgmenu.cpp
    dlgcart.cpp
    dlgpayment.cpp
    dlgpaymentstub.cpp
    dlgorderdone.cpp
    selfboardordersubmit.cpp
    dlgdishdetails.cpp
    dlgpackagepick.cpp
    menuhelpers.cpp
    menustubclient.cpp
    menucache.cpp
    webmenuclient.cpp
    dishcardwidget.cpp
    dishcardanimhost.cpp
    ordercart.cpp
    selfboarddisplay.cpp
    ../NTable/ndataprovider.cpp
    ../NTable/ninterface.cpp
    ../NTable/nloadingdlg.cpp
    ../Classes/logwriter.cpp
    ../Classes/QProgressIndicator.cpp
)

set(HEADERS
    dlgstart.h
    dlgserversettings.h
    serverconfig.h
    appsettings.h
    selfboardsettings.h
    selfboardlanguage.h
    selfboardbottomchrome.h
    cartflyanimation.h
    dlgmenu.h
    dlgcart.h
    dlgpayment.h
    dlgpaymentstub.h
    dlgorderdone.h
    selfboardordersubmit.h
    paymentstubtypes.h
    dlgdishdetails.h
    dlgpackagepick.h
    menuhelpers.h
    menutypes.h
    imenuclient.h
    menustubclient.h
    menucache.h
    webmenuclient.h
    dishcardwidget.h
    dishcardanimhost.h
    ordercart.h
    selfboarddisplay.h
    version.h
    ../Classes/format_bytes.h
)

set(FORMS
    dlgstart.ui
    dlgserversettings.ui
    dlgmenu.ui
    dlgcart.ui
    dlgpayment.ui
    dlgpaymentstub.ui
    ../NTable/nloadingdlg.ui
)

set(TS_FILES
    i18n/SelfBoard_en.ts
    i18n/SelfBoard_ru.ts
    i18n/SelfBoard_hy.ts
)

if(WIN32)
    set(RESOURCES_WIN res.rc)
    set(EXE_TYPE WIN32)
endif()

add_executable(SelfBoard ${EXE_TYPE}
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    selfboard.qrc
    ${RESOURCES_WIN}
)

target_compile_definitions(SelfBoard PRIVATE
    QT_DEPRECATED_WARNINGS
    SELFBOARD
    _ORGANIZATION_="Jazzve"
    _APPLICATION_="SelfBoard"
    _MODULE_="selfboard"
)

set_source_files_properties(../NTable/ninterface.cpp PROPERTIES COMPILE_DEFINITIONS SELFBOARD)

target_link_libraries(SelfBoard PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

qt_add_translations(SelfBoard
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX /lang
)

if(WIN32)
    add_custom_target(selfboard_version_inc
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass
                -File "${CMAKE_CURRENT_SOURCE_DIR}/increase_version.ps1"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Increasing SelfBoard build number..."
    )
    add_dependencies(SelfBoard selfboard_version_inc)
endif()
