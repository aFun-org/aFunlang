/*
 * 文件名: macro.h
 * 目标: 定义公共宏 和 公共头文件
 */

#ifndef AFUN_TOOL_H
#define AFUN_TOOL_H
#include "tool-macro.h"

#ifdef __cplusplus
#include <iostream>
#include <cinttypes>
#include <cstdarg>
#else
#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h>
#endif

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

#ifndef __cplusplus
    #include "stdbool.h"
#endif

    AFUN_STATIC const char NUL = 0;
    typedef uint32_t FileLine;  // 文件行号

#ifdef __cplusplus
    typedef std::string FilePath;  // 文件路径
#endif

#ifndef AFUN_TOOL_C
}
#endif

#include "mem.h"  // 该定义需要放到下面

#endif //AFUN_TOOL_H
