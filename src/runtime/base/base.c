#include "__base.h"

int aFunTool_base(af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    printf("Run runtimeTool-Base %p\n", vs);
    makeLiteralRegexFromList(literal, env);
    makeStrFunc(visitor, vs, env);
    makeQuitFunc(visitor, vs, env);
    return 0;
}