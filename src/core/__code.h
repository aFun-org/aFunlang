/*
 * 文件名: __bytecode.h
 * 目标: 定义Code结构体
 */

#ifndef AFUN__BYTECODE_H
#define AFUN__BYTECODE_H
#include "macro.h"
#include "code.h"

typedef unsigned int CodeUint;  // Code uint

enum af_CodeType {
    literal = 0,
    variable,
    block,  // 括号
};


struct af_Code {  // 一个 Code 的结构体
    enum af_CodeType type;
    char prefix;  // 前缀
    union {
        struct {
            char *literal_data;
            char *func;  // 函数名称
        } literal;

        struct {
            char *name;
        } variable;

        struct {
            CodeUint elements;  // 元素个数
            enum af_BlockType type;  // 括号类型
        } block;
    };

    FileLine line;
    FilePath path;  // path == NULL表示沿用上层地址

    struct af_Code *next;
};

#endif //AFUN__BYTECODE_H
