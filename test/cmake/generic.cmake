try_compile(generic-test ${CMAKE_BINARY_DIR}/try
            SOURCES ${CMAKE_CURRENT_LIST_DIR}/generic.c
            OUTPUT_VARIABLE re
            C_STANDARD 11
            C_STANDARD_REQUIRED TRUE
            C_EXTENSIONS FALSE)

if (NOT generic-test)
    file(WRITE "${CMAKE_BINARY_DIR}/try.txt" ${re})
    message(FATAL_ERROR "C cannot use _Generic exp.\n${re}")
else()
    message(STATUS "C can use _Generic exp.")
endif()