#ifndef AFUN___BASE_H
#define AFUN___BASE_H
#include "__aFun_tool.h"
#include "__env.h"
#include "__var.h"

static char *const string_id = "string";
static char *const string_func_id = "string-maker";

static const LiteralFunc literal[] = {
        /* 字符串匹配：\"[\s\S]*\" */
        {.pattern="\\\"[\\s\\S]*\\\"", .func="str", .in_protect=true},
};

typedef struct ObjectString ObjectString;
struct ObjectString{
    char *str;
};

typedef struct ObjectStrFunc ObjectStrFunc;
struct ObjectStrFunc {
    af_ObjectAPI *api;
    af_VarSpace *share_vs;
    af_VarSpaceListNode *func_var_list;
};

AFUN_LANG_NO_EXPORT void makeStrFunc(af_Object *visitor, af_VarSpace *vs, af_Environment *env);

#endif //AFUN___BASE_H
