cmake_minimum_required(VERSION 3.20)
project(ServerNames LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

# ALL | ELINA | VALSH | DEBUG  — matches the old .pro variants
set(SERVERNAMES_VARIANT "ALL" CACHE STRING "ServerNames build variant: ALL, ELINA, VALSH, DEBUG")
set_property(CACHE SERVERNAMES_VARIANT PROPERTY STRINGS ALL ELINA VALSH DEBUG)

if(SERVERNAMES_VARIANT STREQUAL "ELINA")
    set(_REMOTE_DEFINE REMOTE_ELINA)
elseif(SERVERNAMES_VARIANT STREQUAL "VALSH")
    set(_REMOTE_DEFINE REMOTE_VALSH)
elseif(SERVERNAMES_VARIANT STREQUAL "DEBUG")
    set(_REMOTE_DEFINE REMOTE_DEBUG)
else()
    set(_REMOTE_DEFINE REMOTE_ALL)
endif()

add_library(${PROJECT_NAME} SHARED
    sqlquery.cpp
    sqlquery.h
    ../../../Classes/logwriter.cpp
    ../../../Classes/logwriter.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ServerNames"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Classes
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    SQLQUERY_LIBRARY
    ${_REMOTE_DEFINE}
    _APPLICATION_="Cafe5"
    _MODULE_="Service5_ServerNames_dll"
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /Zc:__cplusplus)
endif()
