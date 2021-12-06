#include "__base.hpp"

int aFunTool_base(af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    writeDebugLog(aFunCoreLogger, "Run runtimeTool-Base %p", vs);

    makeLiteralRegexFromList(literal, env);
    writeDebugLog(aFunCoreLogger, "Run runtimeTool-Base literal success");

    makeStrFunc(visitor, vs, env);
    writeDebugLog(aFunCoreLogger, "Run runtimeTool-Base str success");

    makeQuitFunc(visitor, vs, env);
    writeDebugLog(aFunCoreLogger, "Run runtimeTool-Base quit success");
    return 0;
}