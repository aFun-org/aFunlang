file(GLOB source
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.c)

file(GLOB source_tool
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/aFunTool/*.c)

file(GLOB private_h
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.h)

file(GLOB private_h_core
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/../core/*.h)  # 需要使用 core 的特定头文件 (高级开发)

set(build_include_runtime ${PROJECT_SOURCE_DIR}/include/runtime)
set(install_include_runtime ${INSTALL_INCLUDEDIR})

file(GLOB public_h
     LIST_DIRECTORIES FALSE
     RELATIVE "${build_include_runtime}"
     "${build_include_runtime}/*.h")

set(public_h_build)
set(public_h_install)

foreach(h IN LISTS public_h)
    file(RELATIVE_PATH _path ${CMAKE_CURRENT_LIST_DIR} "${build_include_runtime}/${h}")
    list(APPEND public_h_build   "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${_path}>")  # 相对路径的转换, 此处只能使用相对路径
    list(APPEND public_h_install "$<INSTALL_INTERFACE:${install_include_runtime}/${h}>")
endforeach()


add_library(aFun-xx-libs SHARED "")  # xx表示均为动态链接 core-share-t
add_library(aFun-cx-libs SHARED "")  # cx表示仅core动态链接 core-share-c
add_library(aFun-ct-libs SHARED "")  # ct表示均静态链接 core-static-s

foreach(tgt aFun-xx-libs aFun-cx-libs aFun-ct-libs)
    target_sources(${tgt}
                   PRIVATE ${source} ${source_tool} ${private_h} ${private_h_core}
                   PUBLIC ${public_h_build} ${public_h_install})
    target_include_directories(${tgt}
                               PRIVATE
                                   "${CMAKE_CURRENT_LIST_DIR}/../core"
                                   "${CMAKE_CURRENT_LIST_DIR}"  # aFunTool内的程序需要使用
                               PUBLIC
                                   $<BUILD_INTERFACE:${build_include_runtime}>
                                   $<INSTALL_INTERFACE:${install_include_runtime}>)
    set_target_properties(${tgt} PROPERTIES PUBLIC_HEADER "${public_h_build}")
endforeach()

set_target_properties(aFun-xx-libs PROPERTIES OUTPUT_NAME "aFun-xx")
set_target_properties(aFun-cx-libs PROPERTIES OUTPUT_NAME "aFun-cx")
set_target_properties(aFun-ct-libs PROPERTIES OUTPUT_NAME "aFun-ct")

target_link_libraries(aFun-xx-libs PUBLIC core-shared-t)
target_link_libraries(aFun-cx-libs PUBLIC core-shared-s)
target_link_libraries(aFun-ct-libs PUBLIC core-static-s)

install(TARGETS aFun-xx-libs aFun-cx-libs aFun-ct-libs
        EXPORT aFunlang
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT base-runtime
        ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT dev
        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT base-runtime
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT advanced-dev)