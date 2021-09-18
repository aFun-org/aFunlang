#include "aFunCore.h"
#include "__object.h"

static size_t getSize(af_Object *obj) {
    return sizeof(af_VarSpace *);
}

static void initData(af_Object *obj, af_VarSpace **data, af_Environment *env) {
    *data = makeVarSpace(obj, env);
}

static void freeData(af_Object *obj, af_VarSpace **data, af_Environment *env) {
    freeVarSpace(*data, env);
}

static af_GcList *getGcList(char *id, void *data) {
    return pushGcList(glt_vs, *(af_VarSpace **)data, NULL);
}


static af_VarSpace *getShareVS(af_Object *obj) {
    return *(af_VarSpace **)getObjectData(obj);
}

af_Object *makeGlobalObject(af_Environment *env) {
    af_ObjectAPI *api = makeObjectAPI();
    DLC_SYMBOL(objectAPIFunc) get_size = MAKE_SYMBOL(getSize, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) init_data = MAKE_SYMBOL(initData, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) free_data = MAKE_SYMBOL(freeData, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_share_vs = MAKE_SYMBOL(getShareVS, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
    if (addAPI(get_size, "obj_getDataSize", api) != 1)
        return NULL;
    if (addAPI(init_data, "obj_initData", api) != 1)
        return NULL;
    if (addAPI(free_data, "obj_destructData", api) != 1)
        return NULL;
    if (addAPI(get_share_vs, "obj_getShareVarSpace", api) != 1)
        return NULL;
    if (addAPI(get_gl, "obj_getGcList", api) != 1)
        return NULL;

    FREE_SYMBOL(get_size);
    FREE_SYMBOL(init_data);
    FREE_SYMBOL(free_data);
    FREE_SYMBOL(get_share_vs);
    FREE_SYMBOL(get_gl);

    return makeObject("object", true, api, true, NULL, NULL, env);
}
