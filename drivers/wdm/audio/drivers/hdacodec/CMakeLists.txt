
list(APPEND SOURCE
    adapter.cpp
    common.cpp
    debug.cpp
    probe.cpp)

add_library(hdacodec MODULE ${SOURCE})

target_link_libraries(hdacodec stdunk libcntpr uuid)
set_module_type(hdacodec wdmdriver)
add_importlibs(hdacodec portcls hal ntoskrnl)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(hdacodec PRIVATE -Wno-write-strings -Wno-switch)
endif()

add_cd_file(TARGET hdacodec DESTINATION reactos/system32/drivers FOR all)
add_driver_inf(hdacodec hdacodec.inf)
