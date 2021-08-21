#ifndef AFUN__ARG_H
#define AFUN__ARG_H
#include "macro.h"
#include "arg.h"
#include "__object.h"
#include "__code.h"

struct ArgCodeList {
    void *info;  // info信息
    size_t size;

    struct af_Code *code;
    bool free_code;  // code 是否释放
    bool run_in_func;  // 是否在函数的变量空间内运行

    struct af_Object *result;  // 有gc引用计数
    struct ArgCodeList *next;
};

struct ArgList {
    char *name;
    struct af_Object *obj;  // 有gc引用计数
    struct ArgList *next;
};

#endif //AFUN__ARG_H
