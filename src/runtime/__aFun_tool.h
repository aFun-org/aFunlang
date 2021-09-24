#ifndef AFUN_AFUN_TOOL_H_
#define AFUN_AFUN_TOOL_H_
#include "aFunlang.h"
#include "aFun_tool.h"
#include "__env.h"

/*
 * 返回 (0/1) 执行正常
 * 返回 其他执行错误
 */
typedef int aFunToolFunction(af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env);
typedef struct ToolFunc ToolFunc;
struct ToolFunc {
    char *name;
    aFunToolFunction *func;
};

#endif //AFUN_AFUN_TOOL_H_
