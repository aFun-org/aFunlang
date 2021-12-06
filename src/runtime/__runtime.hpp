#ifndef AFUN_RUNTIME_H_
#define AFUN_RUNTIME_H_
#include "aFunlang.hpp"
#include "runtime.hpp"
#include "__env.hpp"

/*
 * 返回 (0/1) 执行正常
 * 返回 其他执行错误
 */
typedef int aFunToolFunction(af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env);
typedef struct ToolFunc ToolFunc;
struct ToolFunc {
    const char *name;
    aFunToolFunction *func;
};

#endif //AFUN_AFUN_TOOL_H_
