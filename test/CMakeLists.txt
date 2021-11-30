list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(function)

include(export/export.cmake)  # 添加export的测试
if (_build_test)
    add_subdirectory(lib)
    add_subdirectory(src)
endif()