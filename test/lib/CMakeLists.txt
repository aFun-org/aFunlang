file(GLOB src_list RELATIVE ${CMAKE_CURRENT_LIST_DIR}
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

foreach(src IN LISTS src_list)
    cmake_path(GET src STEM file_name)
    string(REPLACE "_" "-" target_name ${file_name})
    add_library(${target_name} SHARED ${src})
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "test_${file_name}")
    define_filename(${target_name})
    unset(file_name)
    unset(target_name)
endforeach()
