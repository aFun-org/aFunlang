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

foreach(tgt tool-shared tool-static)
    target_sources(${tgt} PRIVATE ${source} ${private_h} PUBLIC ${public_h_build} ${public_h_install})
    target_include_directories(${tgt} PUBLIC ${build_include} ${install_include})
    set_target_properties(${tgt} PROPERTIES
                          PUBLIC_HEADER "${public_h_build}")
    define_FILENAME(${tgt})
endforeach()

target_link_libraries(tool-shared PUBLIC ${dlfcn_lib} PCRE2::8BIT Threads::Threads)
target_link_libraries(tool-static PUBLIC ${dlfcn_lib} PCRE2::8BIT Threads::Threads)

set_target_properties(tool-shared PROPERTIES OUTPUT_NAME "aFuntool")
set_target_properties(tool-static PROPERTIES OUTPUT_NAME "aFuntool-s")

install(TARGETS tool-shared tool-static
        EXPORT aFunlang
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT dev
        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT runtime
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT unsafe-dev)