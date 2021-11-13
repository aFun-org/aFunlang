cmake_minimum_required(VERSION 3.20)
project(aFunTest C)

set(CMAKE_C_STANDARD 11)

find_package(aFunlang REQUIRED)
add_executable(aFunTest main.c)

target_link_libraries(aFunTest aFunlang::core-shared)

file(GLOB _dll  # 遍历所有的.dll
     FOLLOW_SYMLINKS  # 遍历link
     LIST_DIRECTORIES FALSE  #  不记录列表
     CONFIGURE_DEPENDS
     "${aFunlang_RUNTIME_DIRS}/*.dll")

if (_dll)
    file(COPY ${_dll} DESTINATION ${CMAKE_BINARY_DIR} USE_SOURCE_PERMISSIONS)
endif()