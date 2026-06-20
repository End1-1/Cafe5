# ZKTeco fingerprint SDK (Windows, import library + headers)

if(NOT WIN32)
    return()
endif()

set(ZTK_ROOT "${CMAKE_CURRENT_LIST_DIR}")

if(NOT EXISTS "${ZTK_ROOT}/lib/libzkfp.lib")
    message(WARNING "ZKTeco SDK not found at ${ZTK_ROOT}/lib/libzkfp.lib")
    return()
endif()

add_library(ZKFingerprint SHARED IMPORTED GLOBAL)

set_target_properties(ZKFingerprint PROPERTIES
    IMPORTED_IMPLIB "${ZTK_ROOT}/lib/libzkfp.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${ZTK_ROOT}/include"
)

if(EXISTS "${ZTK_ROOT}/lib/libzkfp.dll")
    set_target_properties(ZKFingerprint PROPERTIES
        IMPORTED_LOCATION "${ZTK_ROOT}/lib/libzkfp.dll"
    )
    set(ZKFP_RUNTIME_DLL "${ZTK_ROOT}/lib/libzkfp.dll" CACHE INTERNAL "ZKTeco libzkfp.dll")
endif()
