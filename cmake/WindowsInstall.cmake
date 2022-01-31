include_guard(GLOBAL)  # 防止二次导入

# 安装 dll
function(_wi_install_import_inline target run lib)
    if(WIN32 OR CYGWIN)  # 只有windows需要执行该操作 (包括cygwin也需要处理.dll依赖的问题)
        if (CMAKE_BUILD_TYPE)
            string(TOUPPER ${CMAKE_BUILD_TYPE} _build_type)
        else()
            set(_build_type DEBUG)
        endif()

        get_target_property(imp ${target} IMPORTED_IMPLIB)
        get_target_property(imp_t ${target} IMPORTED_IMPLIB_${_build_type})

        get_target_property(loc ${target} IMPORTED_LOCATION)
        get_target_property(loc_t ${target} IMPORTED_LOCATION_${_build_type})

        if(lib)
            if (imp)
                install(FILES ${imp} DESTINATION ${lib})
            endif()
            if (imp_t)
                install(FILES ${imp_t} DESTINATION ${lib})
            endif()
        endif()

        if(run)
            if (loc)
                install(FILES ${loc} DESTINATION ${run})
            endif()
            if (loc_t)
                install(FILES ${loc_t} DESTINATION ${run})
            endif()
        endif()
    endif()
endfunction()

# 拷贝dll
# 添加 target
if (NOT TARGET import_build)
    add_custom_target(import_build ALL COMMENT "Copy import target.")
endif()

macro(set_copy_command target a b)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${a}" "${b}"
            COMMENT "Copy ${a}.")
endmacro()

function(_wi_build_import_inline target run lib)
    if(WIN32 OR CYGWIN)
        if (CMAKE_BUILD_TYPE)
            string(TOUPPER ${CMAKE_BUILD_TYPE} _build_type)
        else()
            set(_build_type DEBUG)
        endif()

        get_target_property(imp ${target} IMPORTED_IMPLIB)
        get_target_property(imp_t ${target} IMPORTED_IMPLIB_${_build_type})

        get_target_property(loc ${target} IMPORTED_LOCATION)
        get_target_property(loc_t ${target} IMPORTED_LOCATION_${_build_type})

        if(lib)
            if (imp)
                set_copy_command(import_build ${imp} ${lib})
            endif()

            if (imp_t)
                set_copy_command(import_build ${imp_t} ${lib})
            endif()
        endif()

        if(run)
            if (loc)
                set_copy_command(import_build ${loc} ${run})
            endif()
            if (loc_t)
                set_copy_command(import_build ${loc_t} ${run})
            endif()
        endif()
    endif()
endfunction()

function(wi_install_import)
    cmake_parse_arguments(ii "" "RUNTIME;LIBRARY" "TARGETS" ${ARGN})
    if (NOT ii_RUNTIME)
        if (INSTALL_BINDIR)
            set(runtime ${INSTALL_BINDIR})
        else()
            set(runtime ${CMAKE_INSTALL_BINDIR})
        endif()
    else()
        set(runtime ${ii_RUNTIME})
    endif()

    if (NOT ii_LIBRARY)
        if (INSTALL_LIBRARY)
            set(library ${INSTALL_LIBRARY})
        else()
            set(library ${CMAKE_INSTALL_LIBDIR})
        endif()
    else()
        set(library ${ii_LIBRARY})
    endif()

    set(targets ${ii_TARGETS})
    foreach(tgt IN LISTS targets) # 不需要 ${}
        _wi_install_import_inline(${tgt} ${runtime} ${library})
        _wi_build_import_inline(${tgt} ${runtime} ${library})
    endforeach()
endfunction()
