#include <stdio.h>
#include "aFun.h"

size_t getSize(af_Object *obj) {
    return sizeof(int *);
}

void initData(af_Object *obj, int **data, af_Environment *env) {
    *data = calloc(1, sizeof(int));
    **data = 100;
}

void freeData(af_Object *obj, int **data, af_Environment *env) {
    printf("freeData(): **data = %d\n", **data);
    free(*data);
}

size_t getSize3(af_Object *obj) {
    return sizeof(af_VarSpace *);
}

void initData3(af_Object *obj, af_VarSpace **data, af_Environment *env) {
    *data = makeVarSpace(obj, 3, 2, 0, env);
}

void freeData3(af_Object *obj, af_VarSpace **data, af_Environment *env) {
    printf("freeData(): *data = %p\n", *data);
    freeVarSpace(*data, env);
}

af_GcList *getGcList3(char *id, void *data) {
    af_GcList *gl = pushGcList(glt_vs, *(af_VarSpace **)data, NULL);
    return gl;
}


af_VarSpace *getShareVS(af_Object *obj) {
    return *(af_VarSpace **)getObjectData(obj);
}

int main() {
    aFunCoreInit();

    af_Environment *env = makeEnvironment(grt_always);
    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) getSize_ = MAKE_SYMBOL(getSize, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_ = MAKE_SYMBOL(initData, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_ = MAKE_SYMBOL(freeData, objectAPIFunc);
        if (addAPI(getSize_, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_, "obj_destructData", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("global", 3, 3, 3,
                                        makeObject("global", true, api, true, NULL, NULL, env), env),
                                env);
        FREE_SYMBOL(getSize_);
        FREE_SYMBOL(initData_);
        FREE_SYMBOL(freeData_);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) getSize_3 = MAKE_SYMBOL(getSize3, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_3 = MAKE_SYMBOL(initData3, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_3 = MAKE_SYMBOL(freeData3, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getShareVS_ = MAKE_SYMBOL(getShareVS, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl3 = MAKE_SYMBOL(getGcList3, objectAPIFunc);
        if (addAPI(getSize_3, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_3, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_3, "obj_destructData", api) != 1)
            return 2;
        if (addAPI(getShareVS_, "obj_getShareVarSpace", api) != 1)
            return 2;
        if (addAPI(get_gl3, "obj_getGcList", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("object", 3, 3, 3,
                                        makeObject("object", true, api, true, NULL, NULL, env),
                                        env),
                                env);
        FREE_SYMBOL(getSize_3);
        FREE_SYMBOL(initData_3);
        FREE_SYMBOL(freeData_3);
        FREE_SYMBOL(getShareVS_);
        FREE_SYMBOL(get_gl3);
    }


    enableEnvironment(env);
    freeEnvironment(env);
    return 0;
}