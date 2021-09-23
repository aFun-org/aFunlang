#ifndef AFUN_AFUN_TOOL_H
#define AFUN_AFUN_TOOL_H

AFUN_LANG_EXPORT int aFunTool(char *name, af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_LANG_EXPORT int aFunToolImport(char *name, af_Object **obj, af_Code **code, af_Environment *env);
#endif //AFUN_AFUN_TOOL_H
