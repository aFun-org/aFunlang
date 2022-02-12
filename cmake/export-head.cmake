include(GenerateExportHeader)

function(define_export_head shared static file_name base_name)
    generate_export_header(${shared}
            EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/${file_name}.h"  # 导出的位置
            BASE_NAME "${base_name}")
    target_compile_definitions(${static} PUBLIC "${base_name}_STATIC_DEFINE")
    set_property(TARGET ${shared}
            PROPERTY C_VISIBILITY_PRESET "hidden")
    set_property(TARGET ${shared}
            PROPERTY VISIBILITY_INLINES_HIDDEN TRUE)
    set_property(TARGET ${static}
            PROPERTY C_VISIBILITY_PRESET "default")
    set_property(TARGET ${static}
            PROPERTY VISIBILITY_INLINES_HIDDEN FALSE)
endfunction()

define_export_head(tool-shared tool-static aFunToolExport AFUN_TOOL)
define_export_head(code-shared code-static aFunCodeExport AFUN_CODE)
define_export_head(core-shared core-static aFunCoreExport AFUN_CORE)
define_export_head(parser-shared parser-static aFunParserExport AFUN_PARSER)
define_export_head(it-shared it-static aFunlangExport AFUN_LANG)

# 两个库需要额外定义
target_compile_definitions(tool-static-c PUBLIC AFUN_TOOL_STATIC_DEFINE)
target_compile_definitions(tool-shared-c PRIVATE tool_shared_EXPORTS)
