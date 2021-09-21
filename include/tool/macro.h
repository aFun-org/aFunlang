﻿/*
 * 文件名: macro.h
 * 目标: 定义公共宏 和 公共头文件
 */

#ifndef AFUN_MACRO_H
#define AFUN_MACRO_H
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>

#ifndef __bool_true_false_are_defined
#define bool int
#define true (1)
#define false (0)
#endif

#define NUL ((char)0)
#define W_NUL ((wchar_t)0)

typedef uint32_t FileLine;  // 文件行号
typedef char *FilePath;  // 文件路径

#endif //AFUN_MACRO_H