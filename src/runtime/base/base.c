#include "__base.h"

int aFunTool_base(af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    writeDebugLog(aFunCoreLogger, log_d, "Run runtimeTool-Base %p", vs);
    makeLiteralRegexFromList(literal, env);
    makeStrFunc(visitor, vs, env);
    makeQuitFunc(visitor, vs, env);
    return 0;
}