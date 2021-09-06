project(af_tool LANGUAGES C)

aux_source_directory(${af_tool_SOURCE_DIR} src)  # 寻找源文件
add_library(af_tool ${src})

set_target_properties(af_tool PROPERTIES OUTPUT_NAME "aFunTool")
target_link_libraries(af_tool dl)

install(TARGETS af_tool)