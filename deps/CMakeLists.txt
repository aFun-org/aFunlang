set(DEPS_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "Deps install.")

set(FFlags_ROOT ${DEPS_INSTALL_PREFIX} CACHE PATH "FFlags root.")
set(dlfcn-win32_ROOT ${DEPS_INSTALL_PREFIX} CACHE PATH "FFlags root.")
set(PCRE2_ROOT ${DEPS_INSTALL_PREFIX} CACHE PATH "pcre2 root.")
set(PCRE2_USE_STATIC_LIBS ON CACHE BOOL "")

if (WIN32 AND NOT CYGWIN)  # cygwin 不依赖 dl
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory dlfcn-win32
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(dlfcn-win32-build)
    add_custom_command(TARGET dlfcn-win32-build POST_BUILD
            COMMAND ${CMAKE_COMMAND}
                    "${CMAKE_CURRENT_LIST_DIR}/dlfcn-win32"
                    "-DCMAKE_INSTALL_PREFIX:PATH=${dlfcn-win32_ROOT}"
                    "-G ${CMAKE_GENERATOR}"
            COMMAND ${CMAKE_COMMAND} --build .
            COMMAND ${CMAKE_COMMAND} --install .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/dlfcn-win32
            COMMENT "Build dlfcn-win32"
	    VERBATIM)
endif()

execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory pcre2
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(PCRE2-build)
add_custom_command(TARGET PCRE2-build POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        "${CMAKE_CURRENT_LIST_DIR}/pcre2"
        "-DCMAKE_INSTALL_PREFIX:PATH=${PCRE2_ROOT}"
        "-DPCRE2_STATIC_PIC=ON"
        "-DPCRE2_BUILD_TESTS=OFF"
        "-DPCRE2_BUILD_PCRE2GREP=OFF"
        "-G" "${CMAKE_GENERATOR}"
        COMMAND ${CMAKE_COMMAND} --build .
        COMMAND ${CMAKE_COMMAND} --install .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pcre2
	COMMENT "Build PCRE2"
	VERBATIM)

