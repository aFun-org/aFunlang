﻿#ifndef AFUN_FUNC_H_
#define AFUN_FUNC_H_
#include "tool.hpp"

#include "func.hpp"
#include "__object.hpp"
#include "__code.hpp"

struct af_ArgCodeList {
    void *info;  // info信息
    size_t size;

    struct af_Code *code;
    bool free_code;  // code 是否释放
    bool run_in_func;  // 是否在函数的变量空间内运行

    struct af_Object *result;  // 有gc引用计数
    struct af_ArgCodeList *next;
};

struct af_ArgList {
    char *name;
    struct af_Object *obj;  // 有gc引用计数
    struct af_ArgList *next;
};

enum af_FuncBodyType {
    func_body_c = 0,  // 回调C函数
    func_body_code,  // 执行af_Code
    func_body_import,  // import内容
    func_body_dynamic,
};

struct af_FuncBody {
    enum af_FuncBodyType type;

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

    bool var_this;  // 是否写入self参数
    bool var_func;  // 是否写入func参数

    // 函数信息
    struct af_FuncBody *body;
};

/* FuncInfo 相关操作 */
AFUN_CORE_NO_EXPORT bool pushDynamicFuncBody(af_FuncBody *new_fb, af_FuncBody *body);

/* ArgList 相关操作 */
AFUN_CORE_NO_EXPORT bool runArgList(af_ArgList *al, af_VarList *vsl, af_Environment *env);

#endif //AFUN_FUNC_H_