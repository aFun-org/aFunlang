#ifndef AFUN_AFUN_TOOL_H
#define AFUN_AFUN_TOOL_H

typedef struct APIFunc APIFunc;
struct APIFunc {
    char *name;
    objectAPIFunc *func;
    DlcHandle *dlc;  // func 的 来源
    DLC_SYMBOL(objectAPIFunc) func_;  // func_和func二选一, func_时dlc无效
    bool free_func_;  // func_是否需要释放
};

typedef struct InheritDefine InheritDefine;
struct InheritDefine {
    af_Object *obj;
};

typedef struct ObjectDefine ObjectDefine;
struct ObjectDefine {
    char *id;

    bool free_api;
    af_ObjectAPI *api;
    struct APIFunc *api_list;  // api和api_list只能二选一

    bool allow_inherit;
    af_Object *belong;
    af_Inherit *inherit;

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
    DlcHandle *dlc;  // func 的 来源
    DLC_SYMBOL(TopMsgProcessFunc) func_;  // func_和func二选一, func_时dlc无效
    bool free_func_;  // func_是否需要释放
};

AFUN_LANG_EXPORT int aFunTool(char *name, af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_LANG_EXPORT int aFunToolImport(char *name, af_Object **obj, af_Code **code, af_Environment *env);

AFUN_LANG_EXPORT af_ObjectAPI *makeAPIFromList(const APIFunc api_list[]);
AFUN_LANG_EXPORT void makeObjectFromList(const ObjectDefine obj_def[], af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_LANG_EXPORT void makeLiteralRegexFromList(const LiteralFunc literal_list[], af_Environment *env);
AFUN_LANG_EXPORT void makeTopMsgProcessFromList(const TopMsgFunc top_msg_list[], af_Environment *env);
AFUN_LANG_EXPORT af_Inherit *makeInheritFromList(const InheritDefine inherit_list[], bool is_reverse);

#endif //AFUN_AFUN_TOOL_H
