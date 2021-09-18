add_subdirectory(tool)
add_subdirectory(core)  # core 依赖 tool

# source在子目录中被使用, 为了避免子目录访问到source, 子目录将在此前面被执行
file(GLOB source
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.c)

file(GLOB private_h
     LIST_DIRECTORIES FALSE
     ${CMAKE_CURRENT_LIST_DIR}/*.h)

file(GLOB include_h
     LIST_DIRECTORIES FALSE
     "${PROJECT_SOURCE_DIR}/include/*.h")

option(AFUN_BUILD_ALL "Build all types of aFun" ON)
set(_build_all ${AFUN_BUILD_ALL})

add_executable(aFun "")
if (${_build_all})
    add_executable(aFun-xx "")  # xx表示均为动态链接 core-share-t
    add_executable(aFun-cx "")  # cx表示仅core动态链接 core-share-c
    add_executable(aFun-ct "")  # ct表示均静态链接 core-static-s
    set(aFunList aFun aFun-xx aFun-cx aFun-ct)
else()
    set(aFunList aFun)
endif()

foreach(tgt IN LISTS aFunList)
    target_sources(${tgt} PRIVATE ${source} ${private_h} ${include_h})
    target_include_directories(${tgt} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    set_target_properties(${tgt} PROPERTIES PUBLIC_HEADER "${include_h}")
endforeach()

set(AFUN_LIBRARY "aFunCore" CACHE STRING "Assign core to aFun")
set(_afun_lib ${AFUN_LIBRARY})
if (_afun_lib STREQUAL "aFunCore-t")
    target_link_libraries(aFun PUBLIC core-shared-t)
elseif(_afun_lib STREQUAL "aFunCore-s")
    target_link_libraries(aFun PUBLIC core-shared-s)
else()
    target_link_libraries(aFun PUBLIC core-static-s)
endif()

install(TARGETS aFun
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT base-runtime
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT advanced-dev)  # 设定安装

if (${_build_all})
    target_link_libraries(aFun-xx PUBLIC core-shared-t)
    target_link_libraries(aFun-cx PUBLIC core-shared-s)
    target_link_libraries(aFun-ct PUBLIC core-static-s)
    install(TARGETS aFun-xx aFun-cx aFun-ct
            RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT advanced-runtim
            PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
            PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT advanced-dev)
endif()
