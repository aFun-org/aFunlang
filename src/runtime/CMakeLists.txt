file(GLOB source
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

file(GLOB source_tool
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*/*.cpp)

file(GLOB private_h
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.hpp)

file(GLOB private_tool_h
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*/*.hpp)

file(GLOB private_h_core
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/../core/*.hpp)  # 需要使用 core 的特定头文件 (高级开发)

file(GLOB public_h
     LIST_DIRECTORIES FALSE
     RELATIVE "${build_include_runtime}"
     "${build_include_runtime}/*.hpp")

set(public_h_build)
set(public_h_install)

foreach(h IN LISTS public_h)
    file(RELATIVE_PATH _path ${CMAKE_CURRENT_LIST_DIR} "${build_include_runtime}/${h}")
    list(APPEND public_h_build   "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${_path}>")  # 相对路径的转换, 此处只能使用相对路径
    list(APPEND public_h_install "$<INSTALL_INTERFACE:${install_include_runtime}/${h}>")
endforeach()


add_library(rt-shared SHARED "")  # xx表示均为动态链接 core-share-t
add_library(rt-static STATIC "")  # ct表示均静态链接 core-static

foreach(tgt rt-shared rt-static)
    target_sources(${tgt}
                   PRIVATE ${source} ${source_tool} ${private_h} ${private_tool_h} ${private_h_core}
                   PUBLIC ${public_h_build} ${public_h_install})
    target_include_directories(${tgt}
                               PRIVATE
                                   "${CMAKE_CURRENT_LIST_DIR}/../core"
                                   "${CMAKE_CURRENT_LIST_DIR}"  # aFunTool内的程序需要使用
                               PUBLIC
                                   ${build_include}
                                   ${install_include})
    set_target_properties(${tgt} PROPERTIES PUBLIC_HEADER "${public_h_build}")
    define_FILENAME(${tgt})
endforeach()

set_target_properties(rt-shared PROPERTIES OUTPUT_NAME "aFunrt")
set_target_properties(rt-static PROPERTIES OUTPUT_NAME "aFunrt-s")

target_link_libraries(rt-shared PUBLIC core-shared)
target_link_libraries(rt-static PUBLIC core-static)

install(TARGETS rt-shared rt-static
        EXPORT aFunlang
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT base-runtime
        ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT dev
        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT base-runtime
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT advanced-dev)