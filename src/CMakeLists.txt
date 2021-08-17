IF (MSVC)
    # 编译dlfcn-win32
    ADD_SUBDIRECTORY(dlfcn)
ENDIF()

ADD_SUBDIRECTORY(tool)
ADD_SUBDIRECTORY(cjson)

SET(libary af_tool af_json)

ADD_SUBDIRECTORY(core)

ADD_EXECUTABLE(aFun main.c)
TARGET_LINK_LIBRARIES(aFun ${libary} af_core)