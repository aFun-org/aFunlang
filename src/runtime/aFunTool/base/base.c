#include "__base.h"

// 桩函数
int aFunTool_base(af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    printf("Run aFunTool-Base %p\n", vs);
    makeLiteralRegexFromList(literal, env);
    makeStrFunc(visitor, vs, env);
    return 0;
}