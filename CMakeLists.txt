cmake_minimum_required(VERSION 3.16)

# 获取版本信息
file(READ ${CMAKE_SOURCE_DIR}/version aFunVersion)
file(READ ${CMAKE_SOURCE_DIR}/version-info aFunDescription)

project(aFun
        VERSION ${aFunVersion}
        DESCRIPTION ${aFunDescription}
        LANGUAGES C CXX)

# CMake系统的相关设定
set(CMAKE_C_STANDARD 11)
set(C_EXTENSIONS OFF)  # 启动编译器拓展
set(C_STANDARD_REQUIRED OFF)

set(CMAKE_CXX_STANDARD 20)
set(CXX_EXTENSIONS OFF)  # 启动编译器拓展
set(CXX_STANDARD_REQUIRED OFF)

set(BUILD_SHARED_LIBS OFF)  # 默认编译静态库 (该设定不可被修改)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type.")
if(CMAKE_BUILD_TYPE EQUAL Debug AND CMAKE_BUILD_TYPE EQUAL Release)
    message(WARNING "Unknown build type.")
endif()

# 系统检查
include(CheckCCompilerFlag)
check_c_compiler_flag("-fPIC" fpic_enable)  # 检查是否有fPIC选项
if (fpic_enable)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()
unset(fpic_enable)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fPIC" fpic_enable)  # 检查是否有fPIC选项
if (fpic_enable)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()
unset(fpic_enable)

if (MSVC)
    if(CMAKE_BUILD_TYPE EQUAL Debug)
        set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /Debug /W4 /wd4251 /wd4275")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Debug /W4 /wd4251 /wd4275")
    else()
        set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W4 /wd4251 /wd4275")
        set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /GR /EHsc /W4 /wd4251 /wd4275")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=UTF-8 -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=UTF-8 -Wall -Wextra -pedantic")
endif()

# 相关参数
set(PRINT_DIR_INFO ON CACHE BOOL "Print install and build dirs info.")
set(_print_dir ${PRINT_DIR_INFO})

include(tool)

include(directory)

if (_print_dir)
    set(PRINT_DIR_INFO OFF CACHE BOOL "Print install and build dirs info." FORCE)
    wi_set_install_dir(NAMES aFunlang)  # 设置安装路径
else()
    wi_set_install_dir_quiet(NAMES aFunlang)  # 设置安装路径
endif()

include(filename)

# CMake对象属性的初始化值的相关设定
set(CMAKE_INSTALL_RPATH
    "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBDIR}"
    "${CMAKE_INSTALL_PREFIX}/${deps_install_dir}/${INSTALL_LIBDIR}")  # INSTALL_RPATH 初始化值
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MACOSX_RPATH TRUE)
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR})  # 添加默认的include路径

add_subdirectory(deps)   # 安装依赖
include(deps)  # 查找依赖
if (NOT DEPS_FOUND)
    return()
endif()

#include(${CMAKE_CURRENT_LIST_DIR}/lang/hgt.cmake)  # 安装lang库
add_subdirectory(src)
include(export-head)

#install(EXPORT aFunlang
#        NAMESPACE "aFunlang::"
#        DESTINATION ${INSTALL_CMAKEDIR}
#        COMPONENT dev)
#
#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(${CMAKE_BINARY_DIR}/cmake-tmp/aFunlangConfigVersion.cmake
#                                 VERSION ${aFunVersion}
#                                 COMPATIBILITY SameMinorVersion)
#
## 生成Config.cmake文件
#set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
#set(BINDIR ${INSTALL_BINDIR})
#set(LIBDIR ${INSTALL_LIBDIR})
#set(CMAKEDIR ${INSTALL_CMAKEDIR})
#set(INCLUDEDIR ${INSTALL_INCLUDEDIR})
#
#if (MSVC)
#    set(pthread_INSTALL_DIR ${deps_install_dir})
#    set(pthread_CMAKEDIR pthread_cmake)
#    set(path_var pthread_INSTALL_DIR pthread_CMAKEDIR)
#else()
#    set(path_var)
#endif()
#if (WIN32 AND NOT CYGWIN)  # cygwin 不需要编译 dl
#    foreach(tgt dlfcn pcre2 fflags)
#        set(${tgt}_INSTALL_DIR ${deps_install_dir})
#        set(${tgt}_CMAKEDIR ${deps_install_dir}/${${tgt}_cmake})
#        list(APPEND path_var ${tgt}_INSTALL_DIR ${tgt}_CMAKEDIR)
#    endforeach()
#else()
#    foreach(tgt pcre2 fflags)
#        set(${tgt}_INSTALL_DIR ${deps_install_dir})
#        set(${tgt}_CMAKEDIR ${deps_install_dir}/${${tgt}_cmake})
#        list(APPEND path_var ${tgt}_INSTALL_DIR ${tgt}_CMAKEDIR)
#    endforeach()
#
#    set(dlfcn_INSTALL_DIR \"\")  # 空字符串
#    set(dlfcn_CMAKEDIR \"\")  # 空字符串
#endif()
#
#configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/aFunlangConfig.cmake.in
#                              ${CMAKE_BINARY_DIR}/cmake-tmp/aFunlangConfig.cmake
#                              INSTALL_DESTINATION ${INSTALL_CMAKEDIR}
#                              PATH_VARS INSTALL_DIR BINDIR LIBDIR CMAKEDIR INCLUDEDIR ${path_var}
#                              INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})  # 指定安装目录 (用于生成相对路径)
#unset(INSTALL_DIR)
#unset(BINDIR)
#unset(LIBDIR)
#unset(CMAKEDIR)
#unset(INCLUDEDIR)
#foreach(tgt dlfcn pcre2 fflags)
#    unset(${tgt}_INSTALL_DIR)
#    unset(${tgt}_CMAKEDIR)
#endforeach()
#unset(path_var)
#
#install(FILES
#        ${CMAKE_BINARY_DIR}/cmake-tmp/aFunlangConfig.cmake
#        ${CMAKE_BINARY_DIR}/cmake-tmp/aFunlangConfigVersion.cmake
#        DESTINATION ${INSTALL_CMAKEDIR})
#

set(BUILD_TEST FALSE CACHE BOOL "Enable run test program")
set(_build_test ${BUILD_TEST})
if (_build_test)
    enable_testing()
endif()
add_subdirectory(test)  # test的相关内容仍会被构建
#
include(info)