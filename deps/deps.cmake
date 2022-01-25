include_guard(GLOBAL)

set(PRINT_DEPS_INFO ON CACHE BOOL "Print deps info.")  # 默认设定为 ON
set(_print ${PRINT_DEPS_INFO})

set(deps_install_dir ${INSTALL_RESOURCEDIR}/deps)  # 依赖的安装位置
set(dlfcn_cmake "share/dlfcn-win32")  # dlfcn cmake 安装位置 (相对路径)
set(fflags_cmake "cmake")  # FindFFlags.cmake 不是 fflags 的一部分, 但是会被安装到 cmake 目录下
set(pthread_cmake "cmake")

set(dlfcn-win32_MUST_BUILD TRUE CACHE BOOL "Must build dlfcn-win32")
set(FFlags_MUST_BUILD TRUE CACHE BOOL "Must build FFlags")
set(PThreadWin32_MUST_BUILD TRUE CACHE BOOL "Must build pthreads-win32")

if (WIN32 AND NOT CYGWIN)  # cygwin 不依赖 dl
    if (_print)
        message(STATUS "Build dlfcn-win32...")
    endif()
    cfep_find_dir(dlfcn-win32
                  REQUIRED
                  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/dlfcn
                  CMAKE_DIR ${dlfcn_cmake}
                  EXTERNAL
                  BUILD_CMAKE_CACHE_ARGS
                    "\"-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}\""
                  BUILD_DIR "dlfcn")
    set(dlfcn_lib dlfcn-win32::dl)
    get_target_property(dlfcn_include_dir dlfcn-win32::dl INTERFACE_INCLUDE_DIRECTORIES)

    get_target_property(_dlfcn_include dlfcn-win32::dl INTERFACE_INCLUDE_DIRECTORIES)  # INTERFACE_INCLUDE_DIRECTORIES
    find_path(dlfcn_h_file NAMES dlfcn.h HINTS ${_dlfcn_include} DOC "The directory of dlfcn.h" REQUIRED NO_DEFAULT_PATH)
    wi_copy_import(TARGETS dlfcn-win32::dl)
    wi_build_import(TARGETS dlfcn-win32::dl)
    wi_install_import(TARGETS dlfcn-win32::dl)
    install(DIRECTORY "${dlfcn_h_file}/" DESTINATION ${INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h")  # 把目录下的内容安装到include
    cfep_install(dlfcn-win32 PREFIX ${deps_install_dir})
else()
    set(DLFCN_ROOT "" CACHE STRING "The directory of dl.")
    set(dlfcn_root ${DLFCN_ROOT})
    if (dlfcn_root)
        find_library(dlfcn_path dl REQUIRED HINTS ${dlfcn_root})
        find_path(dlfcn_include_dir dl.h HINTS ${dlfcn_root})
    else()
        find_library(dlfcn_path dl REQUIRED)
        find_path(dlfcn_include_dir dlfcn.h)
    endif()

    if (NOT dlfcn_include_dir)
        set(dlfcn_include_dir "")
    endif()

    unset(dlfcn_root)
    set(dlfcn_lib ${dlfcn_path})
endif()

if (_print)
    message(STATUS "Build fflags...")
endif()

cfep_find_dir(FFlags
              REQUIRED
              MODULE  # 使用FindFFlags.cmake文件
              SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/fflags
              CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake"  # FindFFlags.cmake 文件位置
              EXTERNAL
              BUILD_CMAKE_ARGS
                  -DBUILD_SHARED_LIBS=OFF
                  -DFFLAGS_BUILD_TEST=OFF
              BUILD_CMAKE_CACHE_ARGS
                "\"-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}\""
              BUILD_DIR "fflags")
set(fflags_lib FFlags::fflags)
get_target_property(fflags_include_dir FFlags::fflags INTERFACE_INCLUDE_DIRECTORIES)

install(DIRECTORY "${fflags_INCLUDE_DIRS}/" DESTINATION ${INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h")  # 安装fflags.h
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/FindFFlags.cmake DESTINATION ${deps_install_dir}/cmake)  # 安装find程序
cfep_install(FFlags PREFIX ${deps_install_dir})

find_package(Threads REQUIRED)
set(thread_lib Threads::Threads)
get_target_property(thread_include_dir Threads::Threads INTERFACE_INCLUDE_DIRECTORIES)
if (NOT thread_include_dir)
    set(thread_include_dir "")
endif()

set(base_include_dir ${dlfcn_include_dir} ${fflags_include_dir} ${thread_include_dir})