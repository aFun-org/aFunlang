IF (MSVC)
    # 编译dlfcn-win32
    ADD_SUBDIRECTORY(deps/dlfcn)
ENDIF()

ADD_SUBDIRECTORY(tool)
ADD_SUBDIRECTORY(deps/cjson)
ADD_SUBDIRECTORY(deps/regex)

SET(libary af_tool af_json  af_regex)

ADD_SUBDIRECTORY(core)

ADD_EXECUTABLE(aFun main.c)
TARGET_LINK_LIBRARIES(aFun ${libary} af_core)