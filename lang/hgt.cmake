find_package(Python3 REQUIRED)

set(hgt_dir ${CMAKE_BINARY_DIR}/hgt)  # hgt 文件储存位置
file(MAKE_DIRECTORY ${hgt_dir})

to_native_path(${hgt_dir} hgt_dir_n)  # hgt 温江储存位置
to_native_path(${CMAKE_CURRENT_LIST_DIR}/tr tr_n)  # tr  文件储存位置
to_native_path(${CMAKE_SOURCE_DIR}/src src_n)  # src 文件储存位置 【需要遍历】
to_native_path(${CMAKE_SOURCE_DIR}/include include_n)  # include 文件位置 【需要遍历】
set(hgt_name aFun)  # ht 名称

set(HGT_COMMAND
    "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/hgt.py"
    ${hgt_dir_n}
    baseHTExport
    BASEHT_EXPORT
    ${tr_n}
    ${hgt_name}
    ${src_n}
    ${include_n})

unset(hgt_dir_n)
unset(tr_n)
unset(src_n)
unset(include_n)

add_custom_target(hgt ALL)
add_custom_command(TARGET hgt POST_BUILD
                   COMMAND ${HGT_COMMAND}
                   COMMENT "The hgt generate file: base"
		   VERBATIM)

execute_process(COMMAND ${HGT_COMMAND}
                RESULT_VARIABLE re
                OUTPUT_VARIABLE output
                ERROR_VARIABLE  errput)

if (re)
    # 运行失败
    message(FATAL_ERROR "hgt error[${re}]\n${errput}")
endif()

add_library(hgt-base SHARED)
target_sources(hgt-base
               PRIVATE ${hgt_dir}/${hgt_name}_ht.c
               PUBLIC $<BUILD_INTERFACE:${hgt_dir}/${hgt_name}_ht.h> $<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}/${hgt_name}_ht.h>)
target_link_libraries(hgt-base ${dlfcn_lib})
add_dependencies(hgt-base hgt)

include(GenerateExportHeader)
generate_export_header(hgt-base EXPORT_FILE_NAME "${hgt_dir}/baseHTExport.h" BASE_NAME "baseHT")

target_include_directories(hgt-base PUBLIC
                           $<BUILD_INTERFACE:${hgt_dir}>
                           $<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>)
set_target_properties(hgt-base PROPERTIES
                      PUBLIC_HEADER "${hgt_dir}/${hgt_name}_ht.h"
                      OUTPUT_NAME "aFunlangTr")

install(TARGETS hgt-base
        EXPORT aFunlang
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT dev
        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT runtime
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT dev
        PRIVATE_HEADER DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT unsafe-dev)

file(GLOB tr
     LIST_DIRECTORIES FALSE
     ${hgt_dir}/tr/*.c)

function(build_lang)
    foreach(src IN LISTS tr)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${INSTALL_LANGDIR})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/${INSTALL_LANGDIR})

        cmake_path(GET src STEM name)
        add_library(hgt-${name} SHARED ${src})
        add_dependencies(hgt-${name} hgt-base)
        add_custom_command(TARGET hgt-${name} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE_BASE_NAME:hgt-${name}>" ">>" "LANG"
                           WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${INSTALL_LANGDIR}"
                           COMMENT "Creat file ${CMAKE_BINARY_DIR}/${INSTALL_LANGDIR}/LANG")

        set_target_properties(hgt-${name} PROPERTIES OUTPUT_NAME "${name}")

        target_compile_definitions(hgt-${name} PRIVATE hgt_base_EXPORTS)
        target_include_directories(hgt-${name} PUBLIC ${hgt_dir})

        install(TARGETS hgt-${name}
                RUNTIME DESTINATION ${INSTALL_LANGDIR} COMPONENT runtime
                LIBRARY DESTINATION ${INSTALL_LANGDIR} COMPONENT runtime)
    endforeach()
endfunction()

build_lang()  # 使用函数防止 CMAKE_RUNTIME_OUTPUT_DIRECTORY 影响外部
unset(hgt-name)
