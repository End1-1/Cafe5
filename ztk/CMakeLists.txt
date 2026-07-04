# ZKTeco fingerprint SDK (Windows, import library + headers)

if(NOT WIN32)
    return()
endif()

set(ZTK_ROOT "${CMAKE_CURRENT_LIST_DIR}")

if(NOT EXISTS "${ZTK_ROOT}/lib/libzkfp.lib")
    message(WARNING "ZKTeco SDK not found at ${ZTK_ROOT}/lib/libzkfp.lib")
    return()
endif()

# Runtime chain for libzkfp.dll (x64): libzkfp -> ZKFPCap.dll, fpslib.dll
set(ZKFP_RUNTIME_DLL_NAMES
    libzkfp.dll
    ZKFPCap.dll
    fpslib.dll
)

set(ZKFP_RUNTIME_DLLS "")
foreach(_zkfp_name IN LISTS ZKFP_RUNTIME_DLL_NAMES)
    set(_zkfp_path "${ZTK_ROOT}/lib/${_zkfp_name}")
    if(EXISTS "${_zkfp_path}")
        list(APPEND ZKFP_RUNTIME_DLLS "${_zkfp_path}")
    endif()
endforeach()

add_library(ZKFingerprint SHARED IMPORTED GLOBAL)

set_target_properties(ZKFingerprint PROPERTIES
    IMPORTED_IMPLIB "${ZTK_ROOT}/lib/libzkfp.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${ZTK_ROOT}/include"
)

if(EXISTS "${ZTK_ROOT}/lib/libzkfp.dll")
    set_target_properties(ZKFingerprint PROPERTIES
        IMPORTED_LOCATION "${ZTK_ROOT}/lib/libzkfp.dll"
    )
endif()

if(ZKFP_RUNTIME_DLLS)
    set(ZKFP_RUNTIME_DLL "${ZTK_ROOT}/lib/libzkfp.dll" CACHE INTERNAL "Primary ZKTeco runtime DLL")
endif()
