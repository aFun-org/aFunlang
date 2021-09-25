#include "__base.h"

#define func_id "quit-func"

typedef struct QuitFunc QuitFunc;
struct QuitFunc {
    af_VarSpaceListNode *func_var_list;
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
    *acl = NULL;
    return true;
}

static bool funcArgList(char *id, af_Object *obj, af_ArgList **al, af_ArgCodeList *acl, void *mark, af_Environment *env) {
    *al = NULL;
    return true;
}

static bool funcVarList(char *id, af_Object *obj, af_VarSpaceListNode **vsl, void *mark, af_Environment *env) {
    QuitFunc *sf = getObjectData(obj);
    *vsl = sf->func_var_list;
    return true;
}

static af_FuncBody *funcBody(void *mark, af_Environment *env) {
    env->core->exit_code = 0;
    env->core->status = core_exit;
    pushMessageDown(makeNORMALMessage(getGlobal(env)), env);
    return NULL;
}

static bool funcGetInfo(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, false, false);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(funcBody, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

static void funcDestruct(char *id, af_Object *obj, QuitFunc *data, af_Environment *env) {
    if (EQ_STR(id, func_id))
        freeAllVarSpaceList(data->func_var_list);
}

void makeQuitFunc(af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    static APIFunc api_list[] = {
            {.name="obj_getDataSize", .func=funcGetSize, .dlc=NULL},
            {.name="obj_initData", .func=funcInit, .dlc=NULL},
            {.name="obj_destructData", .func=funcDestruct, .dlc=NULL},
            {.name="obj_funcGetArgCodeList", .func=funcArgCodeList, .dlc=NULL},
            {.name="obj_funcGetVarList", .func=funcVarList, .dlc=NULL},
            {.name="obj_funcGetArgList", .func=funcArgList, .dlc=NULL},
            {.name="obj_funcGetInfo", .func=funcGetInfo, .dlc=NULL},
            {.name=NULL}
    };

    static ObjectDefine obj_def[] = {
            {.id=func_id, .free_api=true, .api_list=api_list, .allow_inherit=false,
                    .var_name="quit", .p_self=3, .p_posterity=3, .p_external=3},
            {.id=NULL}
    };

    makeObjectFromList(obj_def, visitor, vs, env);
}