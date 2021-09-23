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

typedef struct APIFunc APIFunc;
struct APIFunc {
    char *name;
    objectAPIFunc *func;
};

typedef struct ObjectDefine ObjectDefine;
struct ObjectDefine {
    char *id;

    bool free_api;
    af_ObjectAPI *api;
    struct APIFunc *api_list;  // api和api_list只能二选一

    bool allow_inherit;
    af_Object *belong;
    af_Inherit *inherit;  // TODO-szh 继承表生成工具

    char *var_name;
    char p_self, p_posterity, p_external;

    af_Object **save;  // obj保存位置
};

typedef struct LiteralFunc LiteralFunc;
struct LiteralFunc {
    char *pattern;
    char *func;
    bool in_protect;
};

typedef struct TopMsgFunc TopMsgFunc;
struct TopMsgFunc {
    char *type;
    TopMsgProcessFunc *func;
};

AFUN_LANG_NO_EXPORT af_ObjectAPI *makeAPIFromList(const APIFunc api_list[]);
AFUN_LANG_NO_EXPORT void makeObjectFromList(const ObjectDefine obj_def[], af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_LANG_NO_EXPORT void makeLiteralRegexFromList(const LiteralFunc literal_list[], af_Environment *env);
AFUN_LANG_NO_EXPORT void makeTopMsgProcessFromList(const TopMsgFunc top_msg_list[], af_Environment *env);

#endif //AFUN_AFUN_TOOL_H_
