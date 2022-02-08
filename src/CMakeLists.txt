set(build_include_tool ${PROJECT_SOURCE_DIR}/include/tool)
set(install_include_tool ${INSTALL_INCLUDEDIR})

set(build_include_core ${PROJECT_SOURCE_DIR}/include/core)
set(install_include_core ${INSTALL_INCLUDEDIR})

set(build_include_interface ${PROJECT_SOURCE_DIR}/include/interface)
set(install_include_interface ${INSTALL_INCLUDEDIR})

set(build_include_
    ${hgt_dir}
    ${base_include_dir}
    ${build_include_tool}
    ${build_include_core}
    ${build_include_core_info}
    ${build_include_interface})

set(build_include)
foreach(dir IN LISTS build_include_)
    list(APPEND build_include $<BUILD_INTERFACE:${dir}>)
endforeach()
unset(build_include_)

set(install_include $<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>)

add_subdirectory(tool)
add_subdirectory(core)  # core 依赖 tool
add_subdirectory(interface)  # interface 依赖 core

# source在子目录中被使用, 为了避免子目录访问到source, 子目录将在此前面被执行
file(GLOB source
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

file(GLOB private_h
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.h)

add_executable(aFun-xx "")  # xx表示均为动态链接
add_executable(aFun-ct "" ../include/tool/macro.h)  # ct表示均静态链接
set(aFunList aFun-xx aFun-ct)

foreach(tgt IN LISTS aFunList)
    target_sources(${tgt} PRIVATE ${source} ${private_h} ${include_h})
    target_include_directories(${tgt} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    define_FILENAME(${tgt})
endforeach()

target_link_libraries(aFun-xx PUBLIC rt-shared FFlags::fflags)
target_link_libraries(aFun-ct PUBLIC rt-static FFlags::fflags)

install(TARGETS aFun-xx aFun-ct
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT dev
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev)
