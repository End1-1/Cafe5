cmake_minimum_required(VERSION 3.21)

project(TestFiscal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

set(PRINTTAXN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../desktop.newTax/Src")
if(NOT EXISTS "${PRINTTAXN_DIR}/printtaxn.cpp")
    message(FATAL_ERROR "printtaxn.cpp not found at ${PRINTTAXN_DIR}")
endif()

add_executable(TestFiscal
    main.cpp
    dlgfiscaltest.cpp
    dlgfiscaltest.h
    dlgfiscaltest.ui
    "${PRINTTAXN_DIR}/printtaxn.cpp"
)

target_include_directories(TestFiscal PRIVATE
    "${PRINTTAXN_DIR}"
    "C:/Development/OpenSSL-Win64/include"
)

target_compile_definitions(TestFiscal PRIVATE
    QT_DEPRECATED_WARNINGS
    _ORGANIZATION_="Jazzve"
    _APPLICATION_="TestFiscal"
    _MODULE_="testfiscal"
)

if(MSVC)
    target_compile_options(TestFiscal PRIVATE /utf-8)
endif()

target_link_libraries(TestFiscal PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

if(WIN32)
    target_link_directories(TestFiscal PRIVATE
        "C:/Development/OpenSSL-Win64/lib/VC/x64/MD"
    )
    target_link_libraries(TestFiscal PRIVATE
        libssl
        libcrypto
        ws2_32
        wsock32
        Version
    )
endif()
