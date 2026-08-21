cmake_minimum_required(VERSION 3.21)

project(WaiterDesigner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(WaiterDesigner WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    waitertablestyle.cpp
    waitertablestyle.h
    waitergoodsgroupstyle.cpp
    waitergoodsgroupstyle.h
    waitergoodsdishstyle.cpp
    waitergoodsdishstyle.h
    tablecelldelegate.cpp
    tablecelldelegate.h
)

target_compile_definitions(WaiterDesigner PRIVATE
    QT_DEPRECATED_WARNINGS
    _ORGANIZATION_="Jazzve"
    _APPLICATION_="WaiterDesigner"
    _MODULE_="waiterdesigner"
)

if(MSVC)
    target_compile_options(WaiterDesigner PRIVATE /utf-8)
endif()

target_link_libraries(WaiterDesigner PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
