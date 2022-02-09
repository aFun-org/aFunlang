list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(function)

if (_build_test)
    message(STATUS "Build CTest")
    add_subdirectory(lib)
    add_subdirectory(src)
endif()