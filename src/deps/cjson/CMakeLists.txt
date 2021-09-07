set(BUILD_SHARED_LIBS OFF)
set(CJSON_BUILD_SHARED_LIBS OFF)
set(ENABLE_CJSON_TEST OFF)
set(CMAKE_C_STANDARD) # cJSON支持C90标准 (若set(CMAKE_C_STANDARD 90)在mingw上会报错)
add_subdirectory(cJSON)

project(af_json LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_library(af_json tool.c)
set_target_properties(af_json PROPERTIES OUTPUT_NAME "aFunJson")
target_link_libraries(af_json af_tool cjson)

install(TARGETS af_json)