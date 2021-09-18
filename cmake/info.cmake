include_guard(GLOBAL)

set(PRINT_INFO ON CACHE BOOL "Print system info.")  # 默认设定为 ON 即打印一次
set(_print ${PRINT_INFO})

if (NOT _print)
    return()
endif()

# 显示编译器信息(1)
message(STATUS "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")  # 显示C编译器的路径
message(STATUS "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")  # 显示C编译器的选项
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")  # 当前的构建类型(若为设置则为空)

# 显示CMake信息
message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")  # 显示当前CMake文件的所在目录
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")  # 显示CMake的构建目录

# 显示操作系统信息
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

# 显示编译器信息(2)
message(STATUS "CMAKE_C_COMPILER_ID = ${CMAKE_C_COMPILER_ID}")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Target is 64 bits")
else()
    message(STATUS "Target is 32 bits")
endif()

message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR = ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# 查询主机信息
foreach(key
        IN ITEMS
        NUMBER_OF_LOGICAL_CORES
        NUMBER_OF_PHYSICAL_CORES
        TOTAL_VIRTUAL_MEMORY
        AVAILABLE_VIRTUAL_MEMORY
        TOTAL_PHYSICAL_MEMORY
        AVAILABLE_PHYSICAL_MEMORY
        IS_64BIT
        HAS_FPU
        HAS_MMX
        HAS_MMX_PLUS
        HAS_SSE
        HAS_SSE2
        HAS_SSE_FP
        HAS_SSE_MMX
        HAS_AMD_3DNOW
        HAS_AMD_3DNOW_PLUS
        HAS_IA64
        OS_NAME
        OS_RELEASE
        OS_VERSION
        OS_PLATFORM
        )
    cmake_host_system_information(RESULT re QUERY ${key})
    message(STATUS "cmake_host_system_information ${key} = ${re}")
endforeach()

set(PRINT_INFO OFF CACHE BOOL "Print system info." FORCE)  # 设置 PRINT_INFO 为 OFF