#include "aFunlang.h"
#include "__cycle_obj.h"

size_t getSizec_Cycle(char *id, af_Object *obj) {
    return sizeof(af_VarList *);
}

void initDatac_Cycle(char *id, af_Object *obj, af_VarList **data, af_Environment *env) {
    *data = pushProtectVarList(NULL, env);
}

void freeDatac_Cycle(char *id, af_Object *obj, af_VarList **data, af_Environment *env) {
    freeAllVarSpaceList(*data);
}

typedef struct CycleMark CycleMark;
struct CycleMark {
    af_FuncInfo *fi;
    af_FuncBody *body;
};

af_FuncBody *func_Cycle(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    CycleMark *mark = cfi->mark;
    cfi->body_next = mark->body;
    pushMessageDown(makeNORMALMessage(getGlobal(env), env), env);
    return NULL;
}

bool getInfoc_Cycle(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, CycleMark *mark, af_Environment *env) {
    mark->fi = makeFuncInfo(normal_scope, not_embedded, false, false, false);
    *fi = mark->fi;

    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(func_Cycle, callFuncBody);
    mark->body = makeCFuncBodyToFuncInfo(func, NULL, mark->fi);  // 压入两个相同的 body
    FREE_SYMBOL(func);

    return true;
}

bool getAclc_Cycle(char *id, af_Object *obj, af_ArgCodeList **acl, af_Code *code, CycleMark **mark, af_Environment *env) {
    *acl = NULL;
    *mark = calloc(1, sizeof(CycleMark));
    return true;
}

bool getVslc_Cycle(char *id, af_Object *obj, af_VarList **vsl, CycleMark *mark, af_Environment *env) {
    *vsl = *(af_VarList **)getObjectData(obj);
    return true;
}

af_GcList *getGcListc_Cycle(char *id, af_Object *obj, void *data) {
    af_GcList *gl = pushGcList(glt_vsl, *(af_VarList **)data, NULL);
    return gl;
}

bool getAlc_Cycle(char *id, af_Object *obj, af_ArgList **al, af_ArgCodeList *acl, CycleMark *mark, af_Environment *env) {
    *al = NULL;
    return true;
}

void freeMarkc_Cycle(char *id, af_Object *obj, CycleMark *mark) {
    free(mark);
}

af_Object *makeCycleObject(af_Environment *env) {
    af_ObjectAPI *api = makeObjectAPI();
    DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAclc_Cycle, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVslc_Cycle, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAlc_Cycle, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_info = MAKE_SYMBOL(getInfoc_Cycle, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMarkc_Cycle, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcListc_Cycle, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_size = MAKE_SYMBOL(getSizec_Cycle, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) init_data = MAKE_SYMBOL(initDatac_Cycle, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) free_data = MAKE_SYMBOL(freeDatac_Cycle, objectAPIFunc);
    if (addAPI(get_size, "obj_getDataSize", api) != 1)
        return NULL;
    if (addAPI(init_data, "obj_initData", api) != 1)
        return NULL;
    if (addAPI(free_data, "obj_destructData", api) != 1)
        return NULL;
    if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
        return NULL;
    if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
        return NULL;
    if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
        return NULL;
    if (addAPI(get_info, "obj_funcGetInfo", api) != 1)
        return NULL;
    if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
        return NULL;
    if (addAPI(get_gl, "obj_getGcList", api) != 1)
        return NULL;

    FREE_SYMBOL(get_alc);
    FREE_SYMBOL(get_vsl);
    FREE_SYMBOL(get_al);
    FREE_SYMBOL(get_info);
    FREE_SYMBOL(free_mark);
    FREE_SYMBOL(get_gl);
    FREE_SYMBOL(get_size);
    FREE_SYMBOL(init_data);
    FREE_SYMBOL(free_data);

    return makeObject("func", true, api, true, NULL, true, NULL, env);
}
