project(dl LANGUAGES C)
add_library(dl dlfcn-win32/src/dlfcn.c)
install(TARGETS dl)