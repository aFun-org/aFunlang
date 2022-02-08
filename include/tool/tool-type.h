/*
 * 文件名: macro.h
 * 目标: 定义公共宏 和 公共头文件
 */

#ifndef AFUN_TOOL_TYPE_H
#define AFUN_TOOL_TYPE_H
#include <iostream>
#include <cinttypes>
#include <cstdarg>
#include "macro.h"
#include "macro.h"
#include "aFunbase.h"

namespace aFuntool {
    AFUN_STATIC const char NUL = 0;

    typedef uint32_t FileLine;  // 文件行号
    typedef std::string FilePath;  // 文件路径
}

#include "mem.h"

#endif //AFUN_TOOL_TYPE_H