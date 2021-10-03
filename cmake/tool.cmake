function(to_native_path in out)
    if (WIN32 AND NOT CYGWIN)
        string(REPLACE "/" "\\\\" _out ${in})  # 转义
        set(${out} ${_out} PARENT_SCOPE)
    else()
        set(${out} ${in} PARENT_SCOPE)
    endif()
endfunction()
