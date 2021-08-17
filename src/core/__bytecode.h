/*
 * 文件名: bytecode.h
 * 目标: 定义ByteCode结构体
 */

#ifndef BYTECODE__H
#define BYTECODE__H
#include "macro.h"
#include "bytecode.h"

enum af_ByteCodeType {
    literal = 0,
    variable,
    block,  // 括号
};


struct af_ByteCode {  // 一个 ByteCode 的结构体
    enum af_ByteCodeType type;
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
            ByteCodeUint elements;  // 元素个数
            enum af_BlockType type;  // 括号类型
        } block;
    };

    FileLine line;
    FilePath path;  // path == NULL表示沿用上层地址

    struct af_ByteCode *next;
};

#endif //BYTECODE__H
