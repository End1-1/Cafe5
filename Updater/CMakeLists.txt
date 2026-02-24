cmake_minimum_required(VERSION 3.21)

project(Updater LANGUAGES CXX)

# ---- C++ ----
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Qt ----
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Sql
)

# ---- Sources ----
set(SOURCES
    main.cpp
    updatemanger.cpp
)

set(HEADERS
    updatemanger.h
    version.h
)

# ---- Windows resource ----
if (WIN32)
    set(RESOURCES res.rc)
endif()

add_executable(Updater
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# ---- Link Qt ----
target_link_libraries(Updater PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
)

# ---- Deprecated warnings ----
target_compile_definitions(Updater PRIVATE QT_DEPRECATED_WARNINGS)

# ---- Version auto increment ----
if (WIN32)
    add_custom_target(version_inc
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass
                -File "${CMAKE_CURRENT_SOURCE_DIR}/increase_version.ps1"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_dependencies(Updater version_inc)
endif()
