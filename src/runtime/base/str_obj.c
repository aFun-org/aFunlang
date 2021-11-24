#include "__base.h"

#define string_func_id "string-maker"
typedef struct ObjectStrFunc ObjectStrFunc;
struct ObjectStrFunc {
    af_ObjectAPI *api;
    af_VarSpace *share_vs;
    af_VarSpaceListNode *func_var_list;
};

static size_t strGetSize(char *id, af_Object *obj) {
    return sizeof(ObjectString);
}

static void strInit(char *id, af_Object *obj, ObjectString *data, af_Environment *env) {
    if (EQ_STR(id, string_id))
        data->str = NULL;
}

static void strDestruct(char *id, af_Object *obj, ObjectString *data, af_Environment *env) {
    if (EQ_STR(id, string_id)) {
        free(data->str);
    }
}

static void strLiteral(char *id, af_Object *obj, ObjectString *data, char *str, af_Environment *env) {
    if (!EQ_STR(id, string_id) || data->str != NULL)
        return;
    writeTrackLog(aFunCoreLogger, "strLiteral str = %s, %d", str, strlen(str));
    data->str = NEW_STR(STR_LEN(str) - 2);  // 取出两个引号
    memcpy(data->str, str + 1, (STR_LEN(str) - 2) * sizeof(char));
}

static size_t strFuncGetSize(char *id, af_Object *obj) {
    return sizeof(ObjectStrFunc);
}

static void strFuncInit(char *id, af_Object *obj, ObjectStrFunc *data, af_Environment *env) {
    static const APIFuncList api_list[] = {
            {.name="obj_getDataSize", .func=strGetSize, .dlc=NULL},
            {.name="obj_initData", .func=strInit, .dlc=NULL},
            {.name="obj_destructData", .func=strDestruct, .dlc=NULL},
            {.name="obj_literalSetting", .func=strLiteral, .dlc=NULL},
            {.name=NULL}
    };

    if (!EQ_STR(id, string_func_id))
        return;
    data->func_var_list = copyVarSpaceList(getRunVarSpaceList(env));
    data->share_vs = makeVarSpace(obj, 3, 2, 0, env);
    data->api = makeAPIFromList(api_list);
//    gc_delReference(data->share_vs, env);
}

static bool strFuncArgCodeList(char *id, af_Object *obj, af_ArgCodeList **acl, af_Code *code, void **mark, af_Environment *env) {
    *acl = NULL;
    return true;
}

static bool strFuncArgList(char *id, af_Object *obj, af_ArgList **al, af_ArgCodeList *acl, void *mark, af_Environment *env) {
    *al = NULL;
    return true;
}

static bool strFuncVarList(char *id, af_Object *obj, af_VarSpaceListNode **vsl, void *mark, af_Environment *env) {
    ObjectStrFunc *sf = getObjectData(obj);
    *vsl = sf->func_var_list;
    return true;
}

static af_FuncBody *strFuncBody(af_CallFuncInfo *cfi, af_Environment *env) {
    af_Object *obj = cfi->func;
    ObjectStrFunc *osf = getObjectData(obj);
    af_Object *str = makeObject((char *) string_id, false, osf->api, false, NULL, true, makeInherit(obj), env);
    pushMessageDown(makeNORMALMessage(str, env), env);
    return NULL;
}

static bool strFuncGetInfo(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, false, false);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(strFuncBody, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

static af_GcList *strFuncGetGc(char *id, af_Object *obj, ObjectStrFunc *data) {
    af_GcList *gl = pushGcList(glt_vsl, data->func_var_list, NULL);
    gl = pushGcList(glt_vs, data->share_vs, gl);
    return gl;
}

static void strFuncDestruct(char *id, af_Object *obj, ObjectStrFunc *data, af_Environment *env) {
    if (EQ_STR(id, string_func_id)) {
        freeObjectAPI(data->api);
        freeAllVarSpaceList(data->func_var_list);
    }
}

void makeStrFunc(af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    static APIFuncList api_list[] = {
            {.name="obj_getDataSize", .func=strFuncGetSize, .dlc=NULL},
            {.name="obj_initData", .func=strFuncInit, .dlc=NULL},
            {.name="obj_destructData", .func=strFuncDestruct, .dlc=NULL},
            {.name="obj_funcGetArgCodeList", .func=strFuncArgCodeList, .dlc=NULL},
            {.name="obj_funcGetVarList", .func=strFuncVarList, .dlc=NULL},
            {.name="obj_funcGetArgList", .func=strFuncArgList, .dlc=NULL},
            {.name="obj_funcGetInfo", .func=strFuncGetInfo, .dlc=NULL},
            {.name="obj_getGcList", .func=strFuncGetGc, .dlc=NULL},
            {.name=NULL}
    };

    static ObjectDefineList obj_def[] = {
            {.id=string_func_id, .free_api=true, .api_list=api_list, .allow_inherit=true,
                    .var_name="str", .p_self=3, .p_posterity=3, .p_external=3},
            {.id=NULL}
    };

    makeObjectFromList(obj_def, visitor, vs, env);
}
