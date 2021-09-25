#ifndef AFUN___BASE_H
#define AFUN___BASE_H
#include "__aFun_tool.h"
#include "__env.h"
#include "__var.h"

static const LiteralFunc literal[] = {
        /* 字符串匹配：\"[\s\S]*\" */
        {.pattern="\\\"[\\s\\S]*\\\"", .func="str", .in_protect=true},
};

#define string_id "string"
typedef struct ObjectString ObjectString;
struct ObjectString{
    char *str;
};

AFUN_LANG_NO_EXPORT void makeStrFunc(af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_LANG_NO_EXPORT void makeQuitFunc(af_Object *visitor, af_VarSpace *vs, af_Environment *env);

#endif //AFUN___BASE_H
