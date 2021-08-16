/*
 * 文件名: macro.h
 * 目标: 定义公共宏
 */

#ifndef MACRO__H
#define MACRO__H
#include <stdbool.h>

#ifndef __bool_true_false_are_defined
#define bool int
#define true (1)
#define false (0)
#endif

#define NUL ((char)0)
#define W_NUL ((wchar_t)0)

typedef int fline;
typedef char *fpath;

#endif //MACRO__H