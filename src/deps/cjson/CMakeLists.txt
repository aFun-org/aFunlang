project(cJson LANGUAGES C)
add_library(cJson STATIC cJSON/cJSON.c)


project(af_json LANGUAGES C)
add_library(af_json tool.c)
set_target_properties(af_json PROPERTIES OUTPUT_NAME "aFunJson")
target_link_libraries(af_json af_tool cJson)

install(TARGETS af_json)