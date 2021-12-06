file(GLOB src_list RELATIVE ${CMAKE_CURRENT_LIST_DIR}
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

foreach(src IN LISTS src_list)
    cmake_path(GET src STEM file_name)
    add_executable(${file_name})
    target_sources(${file_name} PRIVATE ${src})
    target_link_libraries(${file_name} PUBLIC rt-static)  # 链接静态库 (导出所有符号)
    set_target_properties(${file_name}
                          PROPERTIES OUTPUT_NAME "test_${file_name}")
    target_compile_definitions(${file_name} PRIVATE IN_CTEST)
    define_FILENAME(${file_name})
    unset(file_name)
endforeach()

# tool 相关测试
file(MD5 "${CMAKE_CURRENT_LIST_DIR}/../share/md5.txt" TOOL_MD5_ANS)
add_new_test(tool_mem COMMAND "$<TARGET_FILE:tool_mem>")
add_new_test(tool_byte COMMAND "$<TARGET_FILE:tool_byte>")
add_new_test(tool_dlc COMMAND "$<TARGET_FILE:tool_byte>" "$<TARGET_FILE:dlc_lib>")
add_new_test(tool_regex COMMAND "$<TARGET_FILE:tool_regex>")
add_new_test(tool_md5 COMMAND "$<TARGET_FILE:tool_md5>" "${TOOL_MD5_ANS}" "${CMAKE_CURRENT_LIST_DIR}/../share/md5.txt")
set_test_label(tool tool_mem tool_byte tool_dlc tool_regex tool_md5)

# core 相关测试
add_new_test(byte_code COMMAND "$<TARGET_FILE:byte_code>")
add_new_test(syntactic COMMAND "$<TARGET_FILE:syntactic>")
set_test_label(io byte_code syntactic)

add_new_test(lexical COMMAND "$<TARGET_FILE:lexical>")
add_new_test(reader COMMAND "$<TARGET_FILE:reader>")
set_test_label(parser lexical reader)

add_new_test(env_init COMMAND "$<TARGET_FILE:env_init>")
add_new_test(run_code COMMAND "$<TARGET_FILE:run_code>")
set_test_label(run env_init run_code)