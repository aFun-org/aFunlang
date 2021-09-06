project(af_core LANGUAGES C)

aux_source_directory(${af_core_SOURCE_DIR} src)  # 寻找源文件
add_library(af_core ${src})

set_target_properties(af_core PROPERTIES OUTPUT_NAME "aFunCore")
target_link_libraries(af_core ${libary})

install(TARGETS af_core)