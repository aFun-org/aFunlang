set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})  # 统一输出到bin目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

FUNCTION(add_aFunTest name src)
    add_executable(aFunTest_${name} ${src})
    target_link_libraries(aFunTest_${name} af_tool af_json af_regex pcre2-8 af_core)
    add_test(NAME aFunTest_${name} COMMAND $<TARGET_FILE:aFunTest_${name}>)
ENDFUNCTION()

FUNCTION(set_pass name pass_regex)
    set_tests_properties(aFunTest_${name} PROPERTIES PASS_REGULAR_EXPRESSION ${pass_regex})
ENDFUNCTION()

FUNCTION(set_link name lib)
    target_link_libraries(aFunTest_${name} ${lib})
ENDFUNCTION()

add_library(lib_Test1 test_lib.c)  # 测试程序需要使用的动态库
set_target_properties(lib_Test1 PROPERTIES OUTPUT_NAME "Test1")

add_definitions(-DLIB_TEST1="$<TARGET_FILE:lib_Test1>")  # 设置宏: lib的所在路径

add_aFunTest(mem test_mem.c)
add_aFunTest(lib test_test_lib.c)
add_aFunTest(dlc test_dlc.c)
add_aFunTest(byte test_byte.c)
add_aFunTest(byte_code test_byte_code.c)
add_aFunTest(env test_env.c)
add_aFunTest(regex test_regex.c)
add_aFunTest(run test_run.c)
add_aFunTest(reader test_reader.c)
add_aFunTest(lexical test_lexcial.c)
add_aFunTest(syntactic test_syntactic.c)

set_link(lib lib_Test1)  # 链接测试程序需要的动态库

set_pass(lib "num = 100 test = 110")
set_pass(dlc "a = 100, test = 110")