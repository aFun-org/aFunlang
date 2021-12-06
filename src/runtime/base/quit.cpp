#include "__base.hpp"

#define func_id "quit-func"

typedef struct QuitFunc QuitFunc;
struct QuitFunc {
    af_VarList *func_var_list;
};

static size_t funcGetSize(char *id, af_Object *obj) {
    return sizeof(QuitFunc);
}

static void funcInit(char *id, af_Object *obj, QuitFunc *data, af_Environment *env) {
    if (!EQ_STR(id, func_id))
        return;
    data->func_var_list = copyVarSpaceList(getRunVarSpaceList(env));
}

static bool funcArgCodeList(char *id, af_Object *obj, af_ArgCodeList **acl, af_Code *code, void **mark, af_Environment *env) {
    *acl = nullptr;
    return true;
}

static bool funcArgList(char *id, af_Object *obj, af_ArgList **al, af_ArgCodeList *acl, void *mark, af_Environment *env) {
    *al = nullptr;
    return true;
}

static bool funcVarList(char *id, af_Object *obj, af_VarList **vsl, void *mark, af_Environment *env) {
    auto sf = (QuitFunc *)getObjectData(obj);
    *vsl = sf->func_var_list;
    return true;
}

static af_FuncBody *funcBody(af_CallFuncInfo *cfi, af_Environment *env) {
    setCoreExit(0, env);
    pushMessageDown(makeNORMALMessage(getGlobal(env), env), env);
    return nullptr;
}

static bool funcGetInfo(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, false, false);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(funcBody, callFuncBody);
    makeCFuncBodyToFuncInfo(func, nullptr, *fi);
    FREE_SYMBOL(func);
    return true;
}

static void funcDestruct(char *id, af_Object *obj, QuitFunc *data, af_Environment *env) {
    if (EQ_STR(id, func_id))
        freeAllVarSpaceList(data->func_var_list);
}

void makeQuitFunc(af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    static APIFuncList api_list[] = {
            {.name="obj_getDataSize", .func=(void *)funcGetSize, .dlc=nullptr},
            {.name="obj_initData", .func=(void *)funcInit, .dlc=nullptr},
            {.name="obj_destructData", .func=(void *)funcDestruct, .dlc=nullptr},
            {.name="obj_funcGetArgCodeList", .func=(void *)funcArgCodeList, .dlc=nullptr},
            {.name="obj_funcGetVarList", .func=(void *)funcVarList, .dlc=nullptr},
            {.name="obj_funcGetArgList", .func=(void *)funcArgList, .dlc=nullptr},
            {.name="obj_funcGetInfo", .func=(void *)funcGetInfo, .dlc=nullptr},
            {.name=nullptr}
    };

    static ObjectDefineList obj_def[] = {
            {.id=func_id, .free_api=true, .api_list=api_list, .allow_inherit=false,
                    .var_name="quit", .p_self=3, .p_posterity=3, .p_external=3},
            {.id=nullptr}
    };

    makeObjectFromList(obj_def, visitor, vs, env);
}