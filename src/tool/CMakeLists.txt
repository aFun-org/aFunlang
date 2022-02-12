set(BUILD_SAFE_MEMORY ON CACHE BOOL "Build safe-memory tool")
set(_build_mem ${BUILD_SAFE_MEMORY})

file(GLOB source
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

file(GLOB private_h
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.h)

file(GLOB public_h
     LIST_DIRECTORIES FALSE
     RELATIVE "${build_include_tool}"
     "${build_include_tool}/*.h")

set(public_h_build)
set(public_h_install)

foreach(h IN LISTS public_h)
    file(RELATIVE_PATH _path ${CMAKE_CURRENT_LIST_DIR} "${build_include_tool}/${h}")
    list(APPEND public_h_build   "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${_path}>")  # 相对路径的转换, 此处只能使用相对路径
    list(APPEND public_h_install "$<INSTALL_INTERFACE:${install_include_tool}/${h}>")
endforeach()

add_library(tool-shared SHARED "")
add_library(tool-static STATIC "")
add_library(tool-shared-c SHARED "")
add_library(tool-static-c STATIC "")

# 基础宏定义
set(definitions
        SYSTEM_NAME="${CMAKE_SYSTEM_NAME}"
        COMPILER_CXX_ID="${CMAKE_CXX_COMPILER_ID}"
        COMPILER_C_ID="${CMAKE_C_COMPILER_ID}"
        AFUN_SHARE_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
        AFUN_SHARE_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
        AFUN_STATIC_PREFIX="${CMAKE_STATIC_LIBRARY_PREFIX}"
        AFUN_STATIC_SUFFIX="${CMAKE_STATIC_LIBRARY_SUFFIX}"
        AFUN_EXE_SUFFIX="${CMAKE_EXECUTABLE_SUFFIX}")
if (WIN32 OR CYGWIN)
    list(APPEND definitions AFUN_WIN32)
endif()
if (WIN32 AND NOT CYGWIN)
    list(APPEND definitions AFUN_WIN32_NO_CYGWIN)
endif()
if (CYGWIN)
    list(APPEND definitions AFUN_CYGWIN)
endif()
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    list(APPEND definitions AFUN_DEBUG)
endif()

foreach(tgt tool-shared tool-static tool-shared-c tool-static-c)
    target_sources(${tgt} PRIVATE ${source} ${private_h} PUBLIC ${public_h_build} ${public_h_install})
    target_include_directories(${tgt} PUBLIC ${build_include} ${install_include})
    target_compile_definitions(${tgt} PUBLIC ${definitions})
    set_target_properties(${tgt} PROPERTIES PUBLIC_HEADER "${public_h_build}" PRIVATE_HEADER "${private_h}")
    target_link_libraries(${tgt} PUBLIC ${dlfcn_lib} PCRE2::8BIT Threads::Threads)
    define_filename(${tgt})
endforeach()

foreach(tgt tool-shared tool-static)
    target_compile_definitions(${tgt} PUBLIC ${definitions})
endforeach()

foreach(tgt tool-shared-c tool-static-c)
    target_compile_definitions(${tgt} PUBLIC ${definitions} AFUN_TOOL_C)
endforeach()

set_target_properties(tool-shared PROPERTIES OUTPUT_NAME "aFuntool")
set_target_properties(tool-static PROPERTIES OUTPUT_NAME "aFuntool-s")
set_target_properties(tool-shared-c PROPERTIES OUTPUT_NAME "aFuntool-c")
set_target_properties(tool-static-c PROPERTIES OUTPUT_NAME "aFuntool-sc")

#install(TARGETS tool-shared tool-static
#        EXPORT aFunlang
#        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT runtime
#        ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT dev
#        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT runtime
#        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
#        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT unsafe-dev)