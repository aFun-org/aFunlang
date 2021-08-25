#ifndef AFUN__FUNC_H
#define AFUN__FUNC_H
#include "macro.h"

typedef struct af_FuncBody af_FuncBody;

#include "func.h"
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

typedef void callFuncBody(void *mark, af_Environment *env);
NEW_DLC_SYMBOL(callFuncBody, callFuncBody);

struct af_FuncBody {
    enum af_FuncBodyType {
        func_body_c,  // 回调C函数
        func_body_code,  // 执行af_Code
    } type;

    union {
        DLC_SYMBOL(callFuncBody) c_func;
        struct {
            af_Code *code;
            bool free_code;
        };
    };

    char **msg_type;  // 表示对应函数可以处理的msg类型
    struct af_FuncBody *next;
};

struct af_FuncInfo {
    // 函数属性
    enum af_FuncInfoScope scope;  // 定义在 func.h
    enum af_FuncInfoEmbedded embedded;  // 定义在 func.h

    bool is_macro;  // 宏函数

    // 函数信息
    struct af_FuncBody *body;
};

#endif //AFUN__FUNC_H
