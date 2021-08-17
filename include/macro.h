/*
 * 文件名: macro.h
 * 目标: 定义公共宏
 */

#ifndef MACRO__H
#define MACRO__H
#include <stdbool.h>
#include <inttypes.h>

#ifndef __bool_true_false_are_defined
#define bool int
#define true (1)
#define false (0)
#endif

#define NUL ((char)0)
#define W_NUL ((wchar_t)0)

typedef int32_t FileLine;  // 文件航海
typedef int8_t *FilePath;  // 文件路径

typedef uint32_t ByteCodeUint;  // ByteCode int

#endif //MACRO__H