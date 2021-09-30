file(GLOB source
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.c)

file(GLOB private_h
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.h)

set(build_include_core ${PROJECT_SOURCE_DIR}/include/core)
set(install_include_core ${INSTALL_INCLUDEDIR})

file(GLOB public_h
     LIST_DIRECTORIES FALSE
     RELATIVE "${build_include_core}"
     "${build_include_core}/*.h")

file(GLOB public_h_info
     LIST_DIRECTORIES FALSE
     RELATIVE "${build_include_core}/info"
     "${build_include_core}/info/*.h")

set(public_h_build)
set(public_h_install)

foreach(h IN LISTS public_h)
    file(RELATIVE_PATH _path ${CMAKE_CURRENT_LIST_DIR} "${build_include_core}/${h}")
    list(APPEND public_h_build   "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${_path}>")  # 相对路径的转换, 此处只能使用相对路径
    list(APPEND public_h_install "$<INSTALL_INTERFACE:${install_include_core}/${h}>")
endforeach()

foreach(h IN LISTS public_h_info)
    file(RELATIVE_PATH _path ${CMAKE_CURRENT_LIST_DIR} "${build_include_core}/info/${h}")
    list(APPEND public_h_build   "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${_path}>")  # 相对路径的转换, 此处只能使用相对路径
    list(APPEND public_h_install "$<INSTALL_INTERFACE:${install_include_core}/${h}>")
endforeach()

add_library(core-shared-t SHARED "")  # core和tool动态库
add_library(core-shared-s SHARED "")  # core动态库, tool静态库
add_library(core-static-s STATIC "")  # core和tool静态库

foreach(tgt core-shared-t core-shared-s core-static-s)
    target_sources(${tgt} PRIVATE ${source} ${private_h} PUBLIC ${public_h_build} ${public_h_install})
    target_include_directories(${tgt} PUBLIC
                               $<BUILD_INTERFACE:${build_include_core}>
                               $<BUILD_INTERFACE:${build_include_core}/info>
                               $<INSTALL_INTERFACE:${install_include_core}>)
    set_target_properties(${tgt} PROPERTIES
                          PUBLIC_HEADER "${public_h_build}"
                          PRIVATE_HEADER "${private_h}")  # PRIVATE_HEADER私有头文件, 可以用于高级开发
    define_FILENAME(${tgt})
endforeach()

set_target_properties(core-shared-t PROPERTIES OUTPUT_NAME "aFunCore-t")
set_target_properties(core-shared-s PROPERTIES OUTPUT_NAME "aFunCore")
set_target_properties(core-static-s PROPERTIES OUTPUT_NAME "aFunCore-static")

target_link_libraries(core-shared-t PUBLIC tool-shared)
target_link_libraries(core-shared-s PUBLIC tool-static)
target_link_libraries(core-static-s PUBLIC tool-static)

install(TARGETS core-shared-t core-shared-s core-static-s
        EXPORT aFunlang
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT base-runtime
        ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT dev
        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT base-runtime
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT advanced-dev)