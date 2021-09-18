try_compile(generic-test ${CMAKE_BINARY_DIR}/try
            SOURCES ${CMAKE_CURRENT_LIST_DIR}/generic.c
            C_STANDARD 11
            C_STANDARD_REQUIRED TRUE
            C_EXTENSIONS FALSE)

if (NOT generic-test)
    message(FATAL_ERROR "C cannot use _Generic exp.")
else()
    message(STATUS "C can use _Generic exp.")
endif()