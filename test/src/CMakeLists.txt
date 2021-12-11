file(GLOB src_list RELATIVE ${CMAKE_CURRENT_LIST_DIR}
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

foreach(src IN LISTS src_list)
    cmake_path(GET src STEM file_name)
    add_executable(${file_name})
    target_sources(${file_name} PRIVATE ${src})
    target_link_libraries(${file_name} PUBLIC tool-static)  # 链接静态库 (导出所有符号)
    set_target_properties(${file_name}
                          PROPERTIES OUTPUT_NAME "test_${file_name}")
    target_compile_definitions(${file_name} PRIVATE IN_CTEST)
    define_FILENAME(${file_name})
    unset(file_name)
endforeach()

# tool 相关测试
file(MD5 "${CMAKE_CURRENT_LIST_DIR}/../share/md5.txt" TOOL_MD5_ANS)
add_new_test(tool_byte COMMAND "$<TARGET_FILE:tool_byte>")
add_new_test(tool_mem COMMAND "$<TARGET_FILE:tool_mem>")
add_new_test(tool_dlc COMMAND "$<TARGET_FILE:tool_byte>" "$<TARGET_FILE:dlc_lib>")
add_new_test(tool_regex COMMAND "$<TARGET_FILE:tool_regex>")
add_new_test(tool_md5 COMMAND "$<TARGET_FILE:tool_md5>" "${TOOL_MD5_ANS}" "${CMAKE_CURRENT_LIST_DIR}/../share/md5.txt")
add_new_test(tool_logger COMMAND "$<TARGET_FILE:tool_logger>")
add_new_test(tool_exit COMMAND "$<TARGET_FILE:tool_exit>")
add_new_test(tool_hash COMMAND "$<TARGET_FILE:tool_hash>")
add_new_test(tool_utf COMMAND "$<TARGET_FILE:tool_utf>")
set_test_label(tool tool_mem tool_byte tool_dlc tool_regex tool_md5 tool_utf)
