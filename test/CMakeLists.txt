list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(function)

if (_build_test)
    add_subdirectory(lib)
    add_subdirectory(src)
endif()