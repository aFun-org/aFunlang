#ifndef AFUN_RUNTIME_H
#define AFUN_RUNTIME_H

typedef struct APIFuncList APIFuncList;
struct APIFuncList {
    char *name;
    void *func;  // objectAPIFunc
    DlcHandle *dlc;  // func 的 来源
    DLC_SYMBOL(objectAPIFunc) func_;  // func_和func二选一, func_时dlc无效
    bool free_func_;  // func_是否需要释放
};

typedef struct InheritDefineList InheritDefineList;
struct InheritDefineList {
    af_Object *obj;
};

typedef struct ObjectDefineList ObjectDefineList;
struct ObjectDefineList {
    char *id;

    bool free_api;
    af_ObjectAPI *api;
    struct APIFuncList *api_list;  // api和api_list只能二选一

    bool allow_inherit;
    af_Object *belong;
    af_Inherit *inherit;

    char *var_name;
    char p_self, p_posterity, p_external;

    af_Object **save;  // obj保存位置
};

typedef struct LiteralFuncList LiteralFuncList;
struct LiteralFuncList {
    char *pattern;
    char *func;
    bool in_protect;
};

typedef struct TopMsgFuncList TopMsgFuncList;
struct TopMsgFuncList {
    char *type;
    TopMsgProcessFunc *func;
    DlcHandle *dlc;  // func 的 来源
    DLC_SYMBOL(TopMsgProcessFunc) func_;  // func_和func二选一, func_时dlc无效
    bool free_func_;  // func_是否需要释放
};

typedef struct GuardianFuncList GuardianFuncList;
struct GuardianFuncList {
    char *type;
    bool always;
    bool derive;
    size_t size;

    DlcHandle *dlc;  // func/destruct 的 来源
    GuardianFunc *func;
    DLC_SYMBOL(GuardianFunc) func_;
    bool free_func_;  // func_是否需要释放

    GuardianDestruct *destruct;
    DLC_SYMBOL(GuardianDestruct) destruct_;
    bool free_destruct_;  // func_是否需要释放

    void **data;
    void (*initData)(void *data, af_Environment *env);  // data初始化的函数
};

AFUN_LANG_EXPORT int runtimeTool(char *name, af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_LANG_EXPORT int runtimeToolImport(char *name, af_Object **obj, af_Code **code, af_Environment *env);

AFUN_LANG_EXPORT af_ObjectAPI *makeAPIFromList(const APIFuncList api_list[]);
AFUN_LANG_EXPORT void makeObjectFromList(const ObjectDefineList obj_def[], af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_LANG_EXPORT void makeLiteralRegexFromList(const LiteralFuncList literal_list[], af_Environment *env);
AFUN_LANG_EXPORT void makeTopMsgProcessFromList(const TopMsgFuncList top_msg_list[], af_Environment *env);
AFUN_LANG_EXPORT af_Inherit *makeInheritFromList(const InheritDefineList inherit_list[], bool is_reverse);
AFUN_LANG_EXPORT void makeGuardianFromList(const GuardianFuncList gd_list[], af_Environment *env);

#endif //AFUN_RUNTIME_H
