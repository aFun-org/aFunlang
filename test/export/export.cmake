add_test(NAME export-install WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         COMMAND ${CMAKE_COMMAND} "--install" "${CMAKE_BINARY_DIR}")

add_test(NAME export-mkdir WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${CMAKE_BINARY_DIR}/export")

if (NOT CMAKE_GENERATOR MATCHES "NMake Makefiles")
    set(CMAKE_ARG "-G" "${CMAKE_GENERATOR}")
else()
    set(CMAKE_ARG)
endif()

add_test(NAME export-test WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/export
         COMMAND ${CMAKE_COMMAND} "${CMAKE_CURRENT_LIST_DIR}"
                                  "-B" "${CMAKE_BINARY_DIR}/export"
                                  ${CMAKE_ARG}
                                  "-DaFunlang_DIR:PATH=${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKEDIR}")

set_test_label(export export-test export-install)

set_tests_properties(export-install PROPERTIES FIXTURES_SETUP "export")  # 添加测试固件
set_tests_properties(export-mkdir PROPERTIES FIXTURES_SETUP "export")  # 添加测试固件
set_tests_properties(export-test PROPERTIES FIXTURES_REQUIRED "export")
