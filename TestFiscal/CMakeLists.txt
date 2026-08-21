cmake_minimum_required(VERSION 3.16)

project(TestFiscal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC OFF)

# Qt 5.15.2 (MSVC2019) — same kit as fiscal / Shop legacy tools
find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Widgets Network)

set(PRINTTAXN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../desktop.newtax/Src")
if(NOT EXISTS "${PRINTTAXN_DIR}/printtaxn.cpp")
    set(PRINTTAXN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../desktop.newTax/Src")
endif()
if(NOT EXISTS "${PRINTTAXN_DIR}/printtaxn.cpp")
    message(FATAL_ERROR "printtaxn.cpp not found (tried desktop.newtax / desktop.newTax)")
endif()

add_executable(TestFiscal WIN32
    main.cpp
    dlgfiscaltest.cpp
    dlgfiscaltest.h
    dlgfiscaltest.ui
    "${PRINTTAXN_DIR}/printtaxn.cpp"
)

set_target_properties(TestFiscal PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_include_directories(TestFiscal PRIVATE
    "${PRINTTAXN_DIR}"
)

target_compile_definitions(TestFiscal PRIVATE
    QT_DEPRECATED_WARNINGS
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    _ORGANIZATION_="Jazzve"
    _APPLICATION_="TestFiscal"
    _MODULE_="testfiscal"
)

if(MSVC)
    # Avoid LNK2019 on operator delete(void*, size_t) with some MSVC/Qt kits.
    target_compile_options(TestFiscal PRIVATE /utf-8 /Zc:__cplusplus /Zc:sizedDealloc-)
    target_link_options(TestFiscal PRIVATE "/DEFAULTLIB:msvcprt")
endif()

target_link_libraries(TestFiscal PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_OPENSSL_ROOT "C:/Development/OpenSSL-Win64")
        set(_OPENSSL_LIBDIR "${_OPENSSL_ROOT}/lib/VC/x64/MD")
    else()
        set(_OPENSSL_ROOT "C:/Development/OpenSSL-Win32")
        set(_OPENSSL_LIBDIR "${_OPENSSL_ROOT}/lib/VC/x86/MD")
    endif()

    set(_OPENSSL_INCLUDE "${_OPENSSL_ROOT}/include")
    if(NOT EXISTS "${_OPENSSL_INCLUDE}/openssl/des.h")
        message(FATAL_ERROR "OpenSSL headers not found: ${_OPENSSL_INCLUDE}/openssl/des.h")
    endif()
    if(NOT EXISTS "${_OPENSSL_LIBDIR}/libssl.lib")
        message(FATAL_ERROR "OpenSSL libs not found: ${_OPENSSL_LIBDIR}/libssl.lib")
    endif()

    target_include_directories(TestFiscal PRIVATE "${_OPENSSL_INCLUDE}")
    target_link_directories(TestFiscal PRIVATE "${_OPENSSL_LIBDIR}")
    target_link_libraries(TestFiscal PRIVATE libssl libcrypto)
    message(STATUS "OpenSSL include: ${_OPENSSL_INCLUDE}")
    message(STATUS "OpenSSL libdir: ${_OPENSSL_LIBDIR}")

    target_link_libraries(TestFiscal PRIVATE
        ws2_32
        wsock32
        crypt32
        Version
    )
endif()
