add_subdirectory(deps/dlfcn)
add_subdirectory(tool)
add_subdirectory(deps/cjson)
add_subdirectory(deps/regex)
add_subdirectory(deps/fflags)

set(libary af_tool af_json af_regex FFlags)

add_subdirectory(core)

add_executable(aFun main.c)
target_link_libraries(aFun ${libary} af_core)