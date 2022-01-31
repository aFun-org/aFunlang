include_guard(GLOBAL)

set(PRINT_DEPS_INFO ON CACHE BOOL "Print deps info.")  # 默认设定为 ON
set(_print ${PRINT_DEPS_INFO})

if (WIN32 AND NOT CYGWIN)  # cygwin 不依赖 dl
    find_package(dlfcn-win32 REQUIRED)
    set(dlfcn_lib dlfcn-win32::dl)
    wi_install_import(TARGETS dlfcn-win32::dl)
else()
    set(DLFCN_ROOT "" CACHE STRING "The directory of dl.")
    set(dlfcn_root ${DLFCN_ROOT})
    if (dlfcn_root)
        find_library(dlfcn_path dl REQUIRED HINTS ${dlfcn_root})
    else()
        find_library(dlfcn_path dl REQUIRED)
    endif()

    unset(dlfcn_root)
    set(dlfcn_lib ${dlfcn_path})
endif()

find_package(FFlags REQUIRED)
find_package(PCRE2 REQUIRED COMPONENTS 8BIT)
find_package(Threads REQUIRED)
wi_install_import(TARGETS FFlags::fflags Threads::Threads PCRE2::8BIT)