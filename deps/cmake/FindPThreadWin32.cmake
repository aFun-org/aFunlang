include(FindPackageHandleStandardArgs)
set(_root ${PThreadWin32_ROOT})

if (NOT _root)
    find_package_handle_standard_args(PThreadWin32
                                      FOUND_VAR PThreadWin32_FOUND
                                      REQUIRED_VARS _root)  # 强制搜不到包
    unset(_root)
endif()

# PThreadWin32_ROOT 必须是 PThreadWin32的安装目录

# PThreadWin32
find_path(pthread_h NAMES pthread.h HINTS ${_root}/include DOC "PThreadWin32 include directory" NO_DEFAULT_PATH)
find_library(pthread_lib NAMES "libpthreadVC3d.lib" "libpthreadVC3.lib"
             libPThreadWin32 HINTS ${_root}/lib DOC "PThreadWin32 library" NO_DEFAULT_PATH)

set(pthread_INCLUDE_DIRS ${pthread_h})
set(pthread_LIBRARIES ${pthread_lib})

unset(pthread_h CACHE)
unset(pthread_lib CACHE)

message(STATUS "pthread_lib = ${pthread_INCLUDE_DIRS}")

find_package_handle_standard_args(PThreadWin32
                                  FOUND_VAR PThreadWin32_FOUND
                                  REQUIRED_VARS
                                  pthread_INCLUDE_DIRS
                                  pthread_LIBRARIES)  # 强制搜不到包

if (PThreadWin32_FOUND)
    add_library(PThreadWin32::pthread STATIC IMPORTED)
    set_target_properties(PThreadWin32::pthread PROPERTIES
                          IMPORTED_LOCATION "${pthread_LIBRARIES}"
                          INTERFACE_INCLUDE_DIRECTORIES "${pthread_INCLUDE_DIRS}"
                          INTERFACE_SOURCES
                          "${pthread_INCLUDE_DIRS}/pthread.h"
                          "${pthread_INCLUDE_DIRS}/sched.h"
                          "${pthread_INCLUDE_DIRS}/semaphore.h"
                          "${pthread_INCLUDE_DIRS}/_ptw32.h"
                          PUBLIC_HEADER
                          "${pthread_INCLUDE_DIRS}/pthread.h"
                          "${pthread_INCLUDE_DIRS}/sched.h"
                          "${pthread_INCLUDE_DIRS}/semaphore.h"
                          "${pthread_INCLUDE_DIRS}/_ptw32.h")
endif()