set(CMAKE_C_STANDARD 11)

project(af_regex LANGUAGES C)

# 在msvc编译PCRE2时, 会遇到`extern char *sys_errlist[];`的相关代码引发错误
# 使用HAVE_STRERROR不编译该部分代码
# 该部分出错代码用于实现GNU的`strerror(int n)`函数


set(PCRE2_SHOW_REPORT OFF)
set(PCRE2_BUILD_TESTS OFF)
set(PCRE2_BUILD_PCRE2GREP OFF)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_INSTALL_LIBDIR  ${LIBRARY_OUTPUT_PATH})
add_definitions(-DHAVE_STRERROR=1)
add_subdirectory(pcre2)

add_library(af_regex regex.c)
set_target_properties(af_regex PROPERTIES OUTPUT_NAME "afRegex")
target_link_libraries(af_regex af_tool pcre2-8-static)

install(TARGETS af_regex)