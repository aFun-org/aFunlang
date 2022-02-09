include_guard(GLOBAL)  # 防止二次导入

# 安装 dll
function(_wi_install_import_inline target run)
    if(WIN32 OR CYGWIN)
        get_target_property(imp ${target} IMPORTED_IMPLIB)
        get_target_property(imp_debug ${target} IMPORTED_IMPLIB_DEBUG)
        get_target_property(imp_release ${target} IMPORTED_IMPLIB_RELEASE)

        get_target_property(loc ${target} IMPORTED_LOCATION)
        get_target_property(loc_debug ${target} IMPORTED_LOCATION_DEBUG)
        get_target_property(loc_release ${target} IMPORTED_LOCATION_RELEASE)

        if(run)
            foreach(tmp ${imp} ${imp_debug} ${imp_release} ${loc} ${loc_debug} ${loc_release})
                if (tmp AND tmp MATCHES ".+dll")
                    install(FILES ${tmp} DESTINATION ${run})
                endif()
            endforeach()
        endif()
    endif()
endfunction()

# 拷贝dll
# 添加 target
if (NOT TARGET deps-copy)
    add_custom_target(deps-copy COMMENT "Copy import target.")
endif()

macro(set_copy_command target a b)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" "-E" "copy_if_different" "${a}" "${b}"
            COMMENT "Copy ${a} to ${b}."
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endmacro()

function(_wi_build_import_inline target run)
    if(WIN32 OR CYGWIN)
        get_target_property(imp ${target} IMPORTED_IMPLIB)
        get_target_property(imp_debug ${target} IMPORTED_IMPLIB_DEBUG)
        get_target_property(imp_release ${target} IMPORTED_IMPLIB_RELEASE)

        get_target_property(loc ${target} IMPORTED_LOCATION)
        get_target_property(loc_debug ${target} IMPORTED_LOCATION_DEBUG)
        get_target_property(loc_release ${target} IMPORTED_LOCATION_RELEASE)

        if(run)
            foreach(tmp ${imp} ${imp_debug} ${imp_release} ${loc} ${loc_debug} ${loc_release})
                if (tmp AND tmp MATCHES ".+dll")
                    set_copy_command(deps-copy ${tmp} ${run})
                endif()
            endforeach()
        endif()
    endif()
endfunction()

function(wi_install_import)
    cmake_parse_arguments(ii "" "RUNTIME" "TARGETS" ${ARGN})
    if (NOT ii_RUNTIME)
        if (INSTALL_BINDIR)
            set(runtime ${INSTALL_BINDIR})
        else()
            set(runtime ${CMAKE_INSTALL_BINDIR})
        endif()
    else()
        set(runtime ${ii_RUNTIME})
    endif()

    set(targets ${ii_TARGETS})
    foreach(tgt IN LISTS targets) # 不需要 ${}
        _wi_install_import_inline(${tgt} ${runtime})
        _wi_build_import_inline(${tgt} ${runtime})
    endforeach()
endfunction()
