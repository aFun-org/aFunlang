/*
 * 文件名: macro.h
 * 目标: 定义公共宏 和 公共头文件
 */

#ifndef AFUN_MACRO_H
#define AFUN_MACRO_H
#include <iostream>
#include <cinttypes>
#include <cstdarg>
#include "aFunbase.h"

namespace aFuntool {
    static const char NUL = 0;

    typedef uint32_t FileLine;  // 文件行号
    typedef char *FilePath;  // 文件路径  (用于多处内存存储场景)
    typedef std::string StringFilePath;  // 文件路径  (用于单处内存存储场景)
    typedef const std::string &ConstFilePath;  // 文件路径  (用于参数)
}

#include "mem.h"

#endif //AFUN_MACRO_H