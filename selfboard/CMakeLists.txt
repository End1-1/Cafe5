cmake_minimum_required(VERSION 3.21)

project(SelfBoard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets LinguistTools)

set(SOURCES
    main.cpp
    dlgstart.cpp
    dlgmenu.cpp
    dlgdishdetails.cpp
    menustubclient.cpp
    dishcardwidget.cpp
    ordercart.cpp
    selfboarddisplay.cpp
)

set(HEADERS
    dlgstart.h
    dlgmenu.h
    dlgdishdetails.h
    menutypes.h
    imenuclient.h
    menustubclient.h
    dishcardwidget.h
    ordercart.h
    selfboarddisplay.h
    version.h
)

set(FORMS
    dlgstart.ui
    dlgmenu.ui
    dlgdishdetails.ui
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

target_link_libraries(SelfBoard PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
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
