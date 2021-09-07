#project(dl LANGUAGES C)
#add_library(dl dlfcn-win32/src/dlfcn.c)
#install(TARGETS dl)

IF (MSVC)
    # 编译dlfcn-win32
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(dlfcn-win32)
ENDIF()