IF (MSVC)
    # 编译dlfcn-win32
    add_subdirectory(deps/dlfcn)
ENDIF()

add_subdirectory(tool)
add_subdirectory(deps/cjson)
add_subdirectory(deps/regex)

set(libary af_tool af_json  af_regex)

add_subdirectory(core)

add_executable(aFun main.c)
target_link_libraries(aFun ${libary} af_core)