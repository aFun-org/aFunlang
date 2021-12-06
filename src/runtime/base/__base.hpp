#ifndef AFUN___BASE_HPP
#define AFUN___BASE_HPP
#include "__runtime.hpp"
#include "__env.hpp"
#include "__var.hpp"

static const LiteralFuncList literal[] = {
        /* 字符串匹配：\"[\s\S]*\" */
        {.pattern="\\\"[\\s\\S]*\\\"", .func="str", .in_protect=true},
        {.pattern=nullptr}
};

#define string_id "string"
typedef struct ObjectString ObjectString;
struct ObjectString{
    char *str;
};

AFUN_LANG_NO_EXPORT void makeStrFunc(af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_LANG_NO_EXPORT void makeQuitFunc(af_Object *visitor, af_VarSpace *vs, af_Environment *env);

#endif //AFUN___BASE_HPP
