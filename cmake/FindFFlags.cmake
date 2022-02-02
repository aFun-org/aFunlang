include(FindPackageHandleStandardArgs)
set(_root ${FFlags_ROOT})

if (NOT _root)
    find_package_handle_standard_args(FFlags
                                      FOUND_VAR FFlags_FOUND
                                      REQUIRED_VARS _root)  # 强制搜不到包
    unset(_root)
endif()

# FFlags_ROOT 必须是 FFlags的安装目录

# FFlags
find_path(fflags_h NAMES fflags.h HINTS ${_root}/include DOC "FFlags include directory" NO_DEFAULT_PATH)
find_library(fflags_lib NAMES FFlags libFFlags HINTS ${_root}/lib DOC "FFlags library" NO_DEFAULT_PATH)

set(fflags_INCLUDE_DIRS ${fflags_h})
set(fflags_LIBRARIES ${fflags_lib})

if (WIN32 AND NOT CYGWIN)
    find_file(fflags_dll NAMES FFlags.dll libFFlags.dll HINTS ${_root}/bin DOC "FFlags ddl" NO_DEFAULT_PATH)
    if (fflags_dll)
        set(fflags_DLL ${fflags_dll})
    else()
        set(fflags_DLL)
    endif()
else()
    set(fflags_DLL ${fflags_LIBRARIES})
endif()

unset(fflags_h CACHE)
unset(fflags_lib CACHE)

find_package_handle_standard_args(FFlags
        FOUND_VAR FFlags_FOUND
        REQUIRED_VARS
        fflags_INCLUDE_DIRS
        fflags_LIBRARIES
        fflags_DLL)  # 强制搜不到包

if (FFlags_FOUND)
    add_library(FFlags::fflags STATIC IMPORTED)
    set_target_properties(FFlags::fflags PROPERTIES
            IMPORTED_IMPLIB "${fflags_DLL}"
            IMPORTED_LOCATION "${fflags_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${fflags_INCLUDE_DIRS}"
            INTERFACE_SOURCES "${fflags_INCLUDE_DIRS}/fflags.h"
            PUBLIC_HEADER "${fflags_INCLUDE_DIRS}/fflags.h")
endif()