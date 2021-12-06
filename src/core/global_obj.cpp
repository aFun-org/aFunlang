#include "aFunCore.hpp"
#include "__object.hpp"

typedef struct GlobalObjectData GlobalObjectData;
struct GlobalObjectData {
    af_VarSpace *share;
};

static const char *global_id = "global-object";

static void initGOD(af_Object *obj, GlobalObjectData *data, af_Environment *env) {
    data->share = makeVarSpace(obj, 3, 2, 0, env);
    gc_delVarSpaceReference(data->share, env);
}

static void freeGOD(GlobalObjectData *god, af_Object  *obj, af_Environment *env) {  // NOLINT 必备参数
    god->share = nullptr;
}

static size_t getSize(char *id, af_Object *obj) {  // NOLINT 必备参数
    /* 不需要检查 id */
    return sizeof(GlobalObjectData);
}

static void initData(char *id, af_Object *obj, GlobalObjectData *data, af_Environment *env) {
    if (EQ_STR(id, global_id))
        initGOD(obj, data, env);
}

static void freeData(char *id, af_Object *obj, GlobalObjectData *data, af_Environment *env) {
    if (EQ_STR(id, global_id))
        freeGOD(data, obj, env);
}

static af_GcList *getGcList(char *id, af_Object *obj, GlobalObjectData *data) {  // NOLINT 必备参数
    if (!EQ_STR(id, global_id))
        return nullptr;

    if (data->share != nullptr)
        return pushGcList(glt_vs, data->share, nullptr);
    return nullptr;
}


static af_VarSpace *getShareVS(char *id, af_Object *obj) {
    if (!EQ_STR(id, global_id))
        return nullptr;
    return ((GlobalObjectData *)getObjectData(obj))->share;
}

af_Object *makeGlobalObject(af_Environment *env) {
    af_ObjectAPI *api = makeObjectAPI();
    DLC_SYMBOL(objectAPIFunc) get_size = MAKE_SYMBOL(getSize, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) init_data = MAKE_SYMBOL(initData, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) free_data = MAKE_SYMBOL(freeData, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_share_vs = MAKE_SYMBOL(getShareVS, objectAPIFunc);
    DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
    if (addAPI(get_size, "obj_getDataSize", api) != 1)
        return nullptr;
    if (addAPI(init_data, "obj_initData", api) != 1)
        return nullptr;
    if (addAPI(free_data, "obj_destructData", api) != 1)
        return nullptr;
    if (addAPI(get_share_vs, "obj_getShareVarSpace", api) != 1)
        return nullptr;
    if (addAPI(get_gl, "obj_getGcList", api) != 1)
        return nullptr;

    FREE_SYMBOL(get_size);
    FREE_SYMBOL(init_data);
    FREE_SYMBOL(free_data);
    FREE_SYMBOL(get_share_vs);
    FREE_SYMBOL(get_gl);

    return makeObject(global_id, true, api, true, nullptr, true, nullptr, env);
}
