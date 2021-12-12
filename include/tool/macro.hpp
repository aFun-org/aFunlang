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

#ifndef __bool_true_false_are_defined
#define bool int
#define true (1)
#define false (0)
#endif

#define NUL ((char)0)
#define W_NUL ((wchar_t)0)

namespace aFuntool {
    typedef uint32_t FileLine;  // 文件行号
    typedef std::string FilePath;  // 文件路径
    typedef const std::string &ConstFilePath;  // 文件路径
}

#endif //AFUN_MACRO_HPP