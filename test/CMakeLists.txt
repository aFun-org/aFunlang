list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(function)

add_subdirectory(cmake)  # cmake配置时测试 必须进行

if (_build_test)
    add_subdirectory(lib)
    add_subdirectory(src)
endif()