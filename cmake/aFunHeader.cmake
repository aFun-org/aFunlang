include(GenerateExportHeader)

generate_export_header(tool-shared
                       EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/aFunToolExport.h"  # 导出的位置
                       BASE_NAME "AFUN_TOOL")

generate_export_header(core-shared-t
                       EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/aFunCoreExport.h"  # 导出的位置
                       BASE_NAME "AFUN_CORE")

generate_export_header(aFun-xx-libs
                       EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/aFunlangExport.h"  # 导出的位置
                       BASE_NAME "AFUN_LANG")

target_compile_definitions(tool-static PUBLIC AFUN_TOOL_STATIC_DEFINE)  # 静态库需要定义 AFUN_TOOL_STATIC_DEFINE

target_compile_definitions(core-shared-s PRIVATE core_shared_t_EXPORTS)
target_compile_definitions(core-static-s PUBLIC AFUN_CORE_STATIC_DEFINE)

target_compile_definitions(aFun-cx-libs PRIVATE aFun_xx_libs_EXPORTS)
target_compile_definitions(aFun-ct-libs PRIVATE aFun_xx_libs_EXPORTS)