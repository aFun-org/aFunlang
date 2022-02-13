set(build_include_tool ${PROJECT_SOURCE_DIR}/include/tool)
set(build_include_code ${PROJECT_SOURCE_DIR}/include/code)
set(build_include_core ${PROJECT_SOURCE_DIR}/include/core)
set(build_include_parser ${PROJECT_SOURCE_DIR}/include/parser)
set(build_include_rt ${PROJECT_SOURCE_DIR}/include/runtime)
set(build_include_interface ${PROJECT_SOURCE_DIR}/include/interface)

set(install_include_tool ${INSTALL_INCLUDEDIR})
set(install_include_code ${INSTALL_INCLUDEDIR})
set(install_include_core ${INSTALL_INCLUDEDIR})
set(install_include_parser ${INSTALL_INCLUDEDIR})
set(install_include_rt ${INSTALL_INCLUDEDIR})
set(install_include_interface ${INSTALL_INCLUDEDIR})

set(build_include)
foreach(dir
        ${hgt_dir}
        ${base_include_dir}
        ${build_include_tool}
        ${build_include_code}
        ${build_include_core}
        ${build_include_parser}
        ${build_include_rt}
        ${build_include_interface})
    list(APPEND build_include $<BUILD_INTERFACE:${dir}>)
endforeach()

set(install_include $<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>)

add_subdirectory(tool)
add_subdirectory(code)
add_subdirectory(core)
add_subdirectory(parser)
add_subdirectory(runtime)
add_subdirectory(interface)

## source在子目录中被使用, 为了避免子目录访问到source, 子目录将在此前面被执行
#file(GLOB source
#     LIST_DIRECTORIES FALSE
#     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
#
#file(GLOB private_h
#     LIST_DIRECTORIES FALSE
#     ${CMAKE_CURRENT_LIST_DIR}/*.h)
#
#add_executable(aFun-xx "")  # xx表示均为动态链接
#add_executable(aFun-ct "")  # ct表示均静态链接
#set(aFunList aFun-xx aFun-ct)
#
#foreach(tgt IN LISTS aFunList)
#    target_sources(${tgt} PRIVATE ${source} ${private_h} ${include_h})
#    target_include_directories(${tgt} PRIVATE ${PROJECT_SOURCE_DIR}/include)
#    target_compile_definitions(${tgt}
#            PRIVATE
#                AFUN_VERSION="${PROJECT_VERSION}"
#                AFUN_DESCRIPTION="${PROJECT_DESCRIPTION}"
#                AFUN_MAJOR_VERSION=${PROJECT_VERSION_MAJOR}
#                AFUN_MINOR_VERSION=${PROJECT_VERSION_MINOR}
#                AFUN_PATCH_VERSION=${PROJECT_VERSION_PATCH})
#    define_filename(${tgt})
#endforeach()
#
#target_link_libraries(aFun-xx PUBLIC it-shared FFlags::fflags)
#target_link_libraries(aFun-ct PUBLIC it-static FFlags::fflags)
#
#install(TARGETS aFun-xx aFun-ct
#        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT dev
#        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
#        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev)
