file(GLOB src_list RELATIVE ${CMAKE_CURRENT_LIST_DIR}
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

foreach(src IN LISTS src_list)
    cmake_path(GET src STEM file_name)
    add_library(${file_name} SHARED ${src})
    set_target_properties(${file_name}
                          PROPERTIES OUTPUT_NAME "test-lib-${file_name}")
    define_FILENAME(${file_name})
    unset(file_name)
endforeach()
