function(add_new_test name)
    set(oneValueArgs WORK)
    set(multiValueArgs COMMAND PROPERTIES)
    cmake_parse_arguments(ant "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ant_WORK)
        set(work ${ant_WORK})
    else()
        set(work ${CMAKE_BINARY_DIR})
    endif()

    add_test(NAME ${name} COMMAND ${ant_COMMAND} WORKING_DIRECTORY ${work})

    if (ant_PROPERTIES)
        set_tests_properties(${name} PROPERTIES ${ant_PROPERTIES})
    endif()
endfunction()

function(set_test_label label)
    set(test ${ARGN})
    set_tests_properties(${test} PROPERTIES LABELS "${label}")
endfunction()
