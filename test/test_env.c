#include <stdio.h>
#include "aFun.h"

size_t getSize(void) {
    return sizeof(int *);
}

void initData(int **data) {
    *data = calloc(sizeof(int), 1);
    **data = 100;
}

void freeData(int **data) {
    printf("**data = %d\n", **data);
    free(*data);
}

int main() {
    aFunInit();

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

        addVarToProtectVarSpace(makeVar("global", 3, 3,
                                        makeObject("global", true, api, true, NULL, NULL, env), env),
                                env);
        FREE_SYMBOL(getSize_);
        FREE_SYMBOL(initData_);
        FREE_SYMBOL(freeData_);
    }
    addVarToProtectVarSpace(makeVar("object", 3, 3,
                                    makeObject("object", true, makeObjectAPI(), true, NULL, NULL, env), env),
                            env);
    if (!enableEnvironment(env)) {
        fprintf(stderr, "Enable Error.\n");
        exit(EXIT_FAILURE);
    }

    freeEnvironment(env);
    return 0;
}