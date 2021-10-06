/*
 * 文件名: __code.h
 * 目标: 定义Code结构体
 */

#ifndef AFUN_BYTECODE_H_
#define AFUN_BYTECODE_H_
#include "tool.h"
#include "code.h"

struct af_Code {  // 一个 Code 的结构体
    enum af_CodeType type;
    char prefix;  // 前缀
    CodeUInt code_end;  // 记录block的end
    union {
        struct {
            char *data;
        } element;

        struct {
            bool is_empty;
            enum af_BlockType type;  // 括号类型
        } block;
    };

    FileLine line;
    FilePath path;  // path == NULL表示沿用上层地址

    struct af_Code *next;
};

#endif //AFUN_BYTECODE_H_
