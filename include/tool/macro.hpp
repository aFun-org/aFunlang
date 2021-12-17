/*
 * 文件名: macro.h
 * 目标: 定义公共宏 和 公共头文件
 */

#ifndef AFUN_MACRO_HPP
#define AFUN_MACRO_HPP
#include <iostream>
#include <cinttypes>
#include <cstdarg>
#include "base.h"

#define NUL ((char)0)
#define W_NUL ((wchar_t)0)

namespace aFuntool {
    typedef uint32_t FileLine;  // 文件行号
    typedef char *FilePath;  // 文件路径  (用于多处内存存储场景)
    typedef std::string StringFilePath;  // 文件路径  (用于单处内存存储场景)
    typedef const std::string &ConstFilePath;  // 文件路径  (用于参数)
}

#endif //AFUN_MACRO_HPP