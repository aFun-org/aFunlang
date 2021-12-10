include(GenerateExportHeader)

# 导出文件保持使用.h后缀 以供C库使用
generate_export_header(tool-shared
                       EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/aFunToolExport.h"  # 导出的位置
                       BASE_NAME "AFUN_TOOL")

#generate_export_header(core-shared
#                       EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/aFunCoreExport.h"  # 导出的位置
#                       BASE_NAME "AFUN_CORE")
#
#generate_export_header(rt-shared
#                       EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/aFunlangExport.h"  # 导出的位置
#                       BASE_NAME "AFUN_LANG")

target_compile_definitions(tool-static PUBLIC AFUN_TOOL_STATIC_DEFINE=1)  # 静态库需要定义 AFUN_TOOL_STATIC_DEFINE
#target_compile_definitions(core-static PUBLIC AFUN_CORE_STATIC_DEFINE=1)
#target_compile_definitions(rt-static PUBLIC AFUN_LANG_STATIC_DEFINE=1)
#
set_property(TARGET tool-shared
             PROPERTY C_VISIBILITY_PRESET "hidden")
set_property(TARGET tool-shared
             PROPERTY VISIBILITY_INLINES_HIDDEN TRUE)

set_property(TARGET tool-static
             PROPERTY C_VISIBILITY_PRESET "default")
set_property(TARGET tool-static
             PROPERTY VISIBILITY_INLINES_HIDDEN FALSE)

#set_property(TARGET tool-shared core-shared rt-shared
#             PROPERTY C_VISIBILITY_PRESET "hidden")
#set_property(TARGET tool-shared core-shared rt-shared
#             PROPERTY VISIBILITY_INLINES_HIDDEN TRUE)
#
#set_property(TARGET tool-static core-static rt-static
#             PROPERTY C_VISIBILITY_PRESET "default")
#set_property(TARGET tool-static core-static
#             PROPERTY VISIBILITY_INLINES_HIDDEN FALSE)