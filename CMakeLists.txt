cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_STANDARD 11)
project(aFun LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)  # 设置输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

set(BUILD_SHARED_LIBS TRUE)  # 默认编译动态库

IF(Build_Memory)
    add_definitions(-DBUILD_MEM=1)
ENDIF()

IF(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    message(STATUS "Build with MSVC")
ELSE()
    add_compile_options(-fPIC)  # 启用fPIC参数
    message(STATUS "Not build with MSVC")
ENDIF()

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")  # 设置安装路径
IF (build_with_rpath)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    message(STATUS "SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) success!")
ENDIF ()

message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_C_COMPILER:   ${CMAKE_C_COMPILER}")

include_directories(${CMAKE_SOURCE_DIR}/include)  # 添加include路径

add_definitions(-DCC="${CMAKE_C_COMPILER}")
add_definitions(-DCXXC="${CMAKE_CXX_COMPILER}")
add_definitions(-DSRC_PATH="${CMAKE_SOURCE_DIR}")

add_subdirectory(src)

IF(ENABLE_aFunTEST)
    enable_testing()
    add_subdirectory(test)
    message(STATUS "Enable Test Success!")
ENDIF()

install(TARGETS aFun)