#include "__base.hpp"

#define string_func_id "string-maker"
typedef struct ObjectStrFunc ObjectStrFunc;
struct ObjectStrFunc {
    af_ObjectAPI *api;
    af_VarSpace *share_vs;
    af_VarList *func_var_list;
};

static size_t strGetSize(char *id, af_Object *obj) {
    return sizeof(ObjectString);
}

static void strInit(char *id, af_Object *obj, ObjectString *data, af_Environment *env) {
    if (EQ_STR(id, string_id))
        data->str = nullptr;
}

static void strDestruct(char *id, af_Object *obj, ObjectString *data, af_Environment *env) {
    if (EQ_STR(id, string_id)) {
        free(data->str);
    }
}

static void strLiteral(char *id, af_Object *obj, ObjectString *data, char *str, af_Environment *env) {
    if (!EQ_STR(id, string_id) || data->str != nullptr)
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
            {.name="obj_getDataSize", .func=(void *)strGetSize, .dlc=nullptr},
            {.name="obj_initData", .func=(void *)strInit, .dlc=nullptr},
            {.name="obj_destructData", .func=(void *)strDestruct, .dlc=nullptr},
            {.name="obj_literalSetting", .func=(void *)strLiteral, .dlc=nullptr},
            {.name=nullptr}
    };

    if (!EQ_STR(id, string_func_id))
        return;
    data->func_var_list = copyVarSpaceList(getRunVarSpaceList(env));
    data->share_vs = makeVarSpace(obj, 3, 2, 0, env);
    data->api = makeAPIFromList(api_list);
    gc_delVarSpaceReference(data->share_vs, env);
}

static bool strFuncArgCodeList(char *id, af_Object *obj, af_ArgCodeList **acl, af_Code *code, void **mark, af_Environment *env) {
    *acl = nullptr;
    return true;
}

static bool strFuncArgList(char *id, af_Object *obj, af_ArgList **al, af_ArgCodeList *acl, void *mark, af_Environment *env) {
    *al = nullptr;
    return true;
}

static bool strFuncVarList(char *id, af_Object *obj, af_VarList **vsl, void *mark, af_Environment *env) {
    auto sf = (ObjectStrFunc *)getObjectData(obj);
    *vsl = sf->func_var_list;
    return true;
}

static af_FuncBody *strFuncBody(af_CallFuncInfo *cfi, af_Environment *env) {
    af_Object *obj = cfi->func;
    auto *osf = (ObjectStrFunc *)getObjectData(obj);
    af_Object *str = makeObject((char *) string_id, false, osf->api, false, nullptr, true, makeInherit(obj), env);
    pushMessageDown(makeNORMALMessage(str, env), env);
    return nullptr;
}

static bool strFuncGetInfo(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, false, false);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(strFuncBody, callFuncBody);
    makeCFuncBodyToFuncInfo(func, nullptr, *fi);
    FREE_SYMBOL(func);
    return true;
}

static af_GcList *strFuncGetGc(char *id, af_Object *obj, ObjectStrFunc *data) {
    af_GcList *gl = pushGcList(glt_vsl, data->func_var_list, nullptr);
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
            {.name="obj_getDataSize", .func=(void *)strFuncGetSize, .dlc=nullptr},
            {.name="obj_initData", .func=(void *)strFuncInit, .dlc=nullptr},
            {.name="obj_destructData", .func=(void *)strFuncDestruct, .dlc=nullptr},
            {.name="obj_funcGetArgCodeList", .func=(void *)strFuncArgCodeList, .dlc=nullptr},
            {.name="obj_funcGetVarList", .func=(void *)strFuncVarList, .dlc=nullptr},
            {.name="obj_funcGetArgList", .func=(void *)strFuncArgList, .dlc=nullptr},
            {.name="obj_funcGetInfo", .func=(void *)strFuncGetInfo, .dlc=nullptr},
            {.name="obj_getGcList", .func=(void *)strFuncGetGc, .dlc=nullptr},
            {.name=nullptr}
    };

    static ObjectDefineList obj_def[] = {
            {.id=string_func_id, .free_api=true, .api_list=api_list, .allow_inherit=true,
                    .var_name="str", .p_self=3, .p_posterity=3, .p_external=3},
            {.id=nullptr}
    };

    makeObjectFromList(obj_def, visitor, vs, env);
}
