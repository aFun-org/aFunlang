#include <stdio.h>
#include "aFun.h"

void mp_ERROR_STR(af_Message *msg, bool is_gc, af_Environment *env) {
    char **pinfo = getMessageData(msg);
    if (!is_gc)
        printf("ERROR\n");
    if (pinfo == NULL || *pinfo == NULL)
        return;
    if (!is_gc)
        fprintf(stderr, "ERROR-STR INFO : %s\n", *pinfo);
    free(*pinfo);
}

size_t getSize(af_Object *obj) {
    return sizeof(int *);
}

void initData(af_Object *obj, int **data, af_Environment *env) {
    *data = calloc(sizeof(int), 1);
    **data = 100;
}

void freeData(af_Object *obj, int **data, af_Environment *env) {
    printf("freeData(): **data = %d\n", **data);
    free(*data);
}

size_t getSize2(af_Object *obj) {
    return sizeof(af_VarSpaceListNode *);
}

void initData2(af_Object *obj, af_VarSpaceListNode **data, af_Environment *env) {
    *data = makeVarSpaceList(getProtectVarSpace(env));
}

void freeData2(af_Object *obj, af_VarSpaceListNode **data, af_Environment *env) {
    printf("freeData2(): vsl = %p\n", *data);
    freeAllVarSpaceList(*data);
}

size_t getSize3(af_Object *obj) {
    return sizeof(af_VarSpace *);
}

void initData3(af_Object *obj, af_VarSpace **data, af_Environment *env) {
    *data = makeVarSpace(obj, env);
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

bool getAcl(af_ArgCodeList **acl, af_Object *obj, af_Code *code, int **mark, af_Environment *env) {
    *acl = makeArgCodeList(makeElementCode("object", NUL, 0, "Unknown"), 0, true, false);
    *mark = calloc(sizeof(int), 1);
    **mark = 100;
    return true;
}

bool getVsl(af_VarSpaceListNode **vsl, af_Object *obj, void *mark, af_Environment *env) {
    *vsl = *(af_VarSpaceListNode **)getObjectData(obj);
    return true;
}

af_GcList *getGcList(char *id, void *data) {
    af_GcList *gl = pushGcList(glt_vsl, *(af_VarSpaceListNode **)data, NULL);
    return gl;
}

bool getAl(af_ArgList **al, af_Object *obj, af_ArgCodeList *acl, void *mark, af_Environment *env) {
    *al = makeArgList("test", getArgCodeListResult(acl));
    return true;
}

void literalSet(char *str, void *data, af_Object *obj, af_Environment *env) {
    printf("literalSet(): str = %s\n", str);
}

af_FuncBody *testFunc(int *mark, af_Environment *env) {  // 测试用函数
    printf("testFunc(): I am testFunc\n");
    af_Object *obj;

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) literal_set = MAKE_SYMBOL(literalSet, objectAPIFunc);
        if (addAPI(literal_set, "obj_literalSetting", api) != 1)
            return NULL;
        obj = makeObject("func", true, api, true, NULL, NULL, env);
        FREE_SYMBOL(literal_set);
    }

    pushMessageDown(makeNORMALMessage(obj), env);
    return NULL;
}

bool getInfo(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknow"), true, NULL, *fi);

    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

void freeMark(int *mark) {
    printf("freeMark(): mark = %d\n", *mark);
    free(mark);
}

af_FuncBody *testFunc2(int *mark, af_Environment *env) {  // 测试用函数
    printf("testFunc2(): I am testFunc2\n");
    af_Object *obj;

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info = MAKE_SYMBOL(getInfo, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return NULL;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return NULL;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
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

        obj = makeObject("func", true, api, true, NULL, NULL, env);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
    }

    pushMessageDown(makeNORMALMessage(obj), env);
    return NULL;
}

bool getInfo2(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, true, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknow"), true, NULL, *fi);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc2, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

bool getInfo3(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("data3", NUL, 0, "Unknow"), true, NULL, *fi);
    return true;
}

af_FuncBody *testFunc4(int *mark, af_Environment *env) {  // 测试用函数
    printf("testFunc4(): I am testFunc4\n");
    af_Object *obj;

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) literal_set = MAKE_SYMBOL(literalSet, objectAPIFunc);
        if (addAPI(literal_set, "obj_literalSetting", api) != 1)
            return NULL;
        obj = makeObject("func", true, api, true, NULL, NULL, env);
        FREE_SYMBOL(literal_set);
    }

    pushMessageDown(makeNORMALMessage(obj), env);
    return NULL;
}

bool getInfo4(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc4, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

af_FuncBody *testFunc9(int *mark, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_FuncBody *fb;
    obj = makeObject("obj", true, makeObjectAPI(), true, NULL, NULL, env);
    pushMessageDown(makeNORMALMessage(obj), env);
    printf("testFunc9(%p): I am testFunc9\n", obj);

    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc9, callFuncBody);
    fb = makeCFuncBody(func1, NULL);
    FREE_SYMBOL(func1);
    
    return fb;
}

bool getInfo9(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc9, callFuncBody);
    makeCFuncBodyToFuncInfo(func1, NULL, *fi);
    FREE_SYMBOL(func1);

    makeDynamicFuncBodyToFuncInfo(*fi);
    return true;
}

af_FuncBody *testFunc8(int *mark, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    obj = makeObject("obj", true, makeObjectAPI(), true, NULL, NULL, env);
    pushMessageDown(makeNORMALMessage(obj), env);
    printf("testFunc8(%p): I am testFunc8\n", obj);
    fflush(stdout);
    return NULL;
}

af_FuncBody *testFunc7(int *mark, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    obj = makeObject("func", true, makeObjectAPI(), true, NULL, NULL, env);
    pushMessageDown(makeNORMALMessage(obj), env);
    printf("testFunc7[des](%p): I am testFunc7\n", obj);
    fflush(stdout);
    return NULL;
}

bool getInfo7(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc7, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

af_FuncBody *testFunc6(int *mark, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_Object *des;
    obj = makeObject("func", true, makeObjectAPI(), true, NULL, NULL, env);

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info7 = MAKE_SYMBOL(getInfo7, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return NULL;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return NULL;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return NULL;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return NULL;
        if (addAPI(get_info7, "obj_funcGetInfo", api) != 1)
            return NULL;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return NULL;
        des = makeObject("func-des", true, api, true, NULL, NULL, env);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_info7);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
    }

    setObjectAttributes(mg_gc_destruct, 3, 3, 3, des, obj, obj, env);
    pushMessageDown(makeNORMALMessage(obj), env);
    printf("testFunc6[des](%p, %p): I am testFunc6\n", obj, des);
    return NULL;
}

bool getInfo6(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc6, callFuncBody);
    makeCFuncBodyToFuncInfo(func1, NULL, *fi);
    FREE_SYMBOL(func1);

    DLC_SYMBOL(callFuncBody) func2 = MAKE_SYMBOL(testFunc8, callFuncBody);
    makeCFuncBodyToFuncInfo(func2, NULL, *fi);
    FREE_SYMBOL(func2);
    return true;
}

af_FuncBody *testFunc5(int *mark, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_Object *des;
    obj = makeObject("func", true, makeObjectAPI(), true, NULL, NULL, env);

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info6 = MAKE_SYMBOL(getInfo6, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return NULL;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return NULL;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return NULL;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return NULL;
        if (addAPI(get_info6, "obj_funcGetInfo", api) != 1)
            return NULL;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return NULL;
        des = makeObject("func-des", true, api, true, NULL, NULL, env);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_info6);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
    }

    setObjectAttributes(mg_gc_destruct, 3, 3, 3, des, obj, obj, env);
    pushMessageDown(makeNORMALMessage(obj), env);
    printf("testFunc5(%p, %p): I am testFunc5\n", obj, des);
    return NULL;
}

bool getInfo5(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc5, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknow"), true, NULL, *fi);
    return true;
}

bool objFunc(af_Object *obj) {
    return true;
}

bool infixFunc(af_Object *obj) {
    return true;
}

int main() {
    aFunInit();
    printf("Hello World\n");

    af_Environment *env = makeEnvironment(grt_always);
    if(!pushLiteralRegex("data.*", "func", true, env)) {
        fprintf(stderr, "pushLiteralRegex Error\n");
        return 1;
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
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
                                        (obj = makeObject("object", true, api, true, NULL, NULL, env)),
                                        env),
                                env);
        FREE_SYMBOL(getSize_3);
        FREE_SYMBOL(initData_3);
        FREE_SYMBOL(freeData_3);
        FREE_SYMBOL(getShareVS_);
        FREE_SYMBOL(get_gl3);
        printf("object(%p)\n", obj);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info = MAKE_SYMBOL(getInfo, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return 2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            return 2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return 2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            return 2;
        if (addAPI(get_info, "obj_funcGetInfo", api) != 1)
            return 2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            return 2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("func", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, NULL, NULL, env)), env),
                                env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func(%p)\n", obj);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info2 = MAKE_SYMBOL(getInfo2, objectAPIFunc);  // 宏函数
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return 2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return 2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            return 2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return 2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            return 2;
        if (addAPI(get_info2, "obj_funcGetInfo", api) != 1)
            return 2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("func2", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, NULL, NULL, env)), env),
                                env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info2);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func2(%p)\n", obj);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info3 = MAKE_SYMBOL(getInfo3, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return 2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return 2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            return 2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return 2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            return 2;
        if (addAPI(get_info3, "obj_funcGetInfo", api) != 1)
            return 2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("func3", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, NULL, NULL, env)), env),
                                env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info3);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func3(%p)\n", obj);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info4 = MAKE_SYMBOL(getInfo4, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) obj_func = MAKE_SYMBOL(objFunc, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return 2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return 2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            return 2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return 2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            return 2;
        if (addAPI(get_info4, "obj_funcGetInfo", api) != 1)
            return 2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            return 2;
        if (addAPI(obj_func, "obj_isObjFunc", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("func4", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, NULL, NULL, env)), env),
                                env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info4);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(obj_func);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func4(%p)\n", obj);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info5 = MAKE_SYMBOL(getInfo5, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return 2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            return 2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return 2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            return 2;
        if (addAPI(get_info5, "obj_funcGetInfo", api) != 1)
            return 2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            return 2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("func5", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, NULL, NULL, env)), env),
                                env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info5);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func5(%p)\n", obj);
    }

    {
        af_Object *obj = makeObject("func", true, makeObjectAPI(), true, NULL, NULL, env);
        af_Object *des;

        {
            af_ObjectAPI *api = makeObjectAPI();
            DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
            DLC_SYMBOL(objectAPIFunc) get_info6 = MAKE_SYMBOL(getInfo6, objectAPIFunc);
            DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
            DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
            DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
            DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
            if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
                return 2;
            if (addAPI(initData_2, "obj_initData", api) != 1)
                return 2;
            if (addAPI(freeData_2, "obj_destructData", api) != 1)
                return 2;
            if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
                return 2;
            if (addAPI(get_info6, "obj_funcGetInfo", api) != 1)
                return 2;
            if (addAPI(get_gl, "obj_getGcList", api) != 1)
                return 2;
            des = makeObject("func-des", true, api, true, NULL, NULL, env);
            FREE_SYMBOL(get_vsl);
            FREE_SYMBOL(get_info6);
            FREE_SYMBOL(get_gl);
            FREE_SYMBOL(getSize_2);
            FREE_SYMBOL(initData_2);
            FREE_SYMBOL(freeData_2);
        }

        setObjectAttributes(mg_gc_destruct, 3, 3, 3, des, obj, obj, env);
        addVarToProtectVarSpace(makeVar("func6", 3, 3, 3, obj, env), env);
        printf("func6(%p)\n", obj);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info9 = MAKE_SYMBOL(getInfo9, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return 2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            return 2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return 2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            return 2;
        if (addAPI(get_info9, "obj_funcGetInfo", api) != 1)
            return 2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            return 2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("func7", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, NULL, NULL, env)), env),
                                env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info9);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func7(%p)\n", obj);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info = MAKE_SYMBOL(getInfo, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) infix_func = MAKE_SYMBOL(infixFunc, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return 2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            return 2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return 2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            return 2;
        if (addAPI(get_info, "obj_funcGetInfo", api) != 1)
            return 2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            return 2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return 2;
        if (addAPI(infix_func, "obj_isInfixFunc", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("func8", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, NULL, NULL, env)), env),
                                env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        FREE_SYMBOL(infix_func);
        printf("func8(%p)\n", obj);
    }

    printf("\n");

    {
        DLC_SYMBOL(TopMsgProcessFunc) func = MAKE_SYMBOL(mp_ERROR_STR, TopMsgProcessFunc);
        addTopMsgProcess("ERROR-STR", func, env);
        FREE_SYMBOL(func);
    }

    enableEnvironment(env);

    {  // 正常程序
        printf("TAG A:\n");
        af_Code *bt1 = makeElementCode("object", 0, 1, NULL);
        af_Code *bt2 = makeElementCode("data", ',', 0, "Unknow");
        connectCode(&bt1, bt2);

        af_Code *bt3 = makeElementCode("func", 0, 1, NULL);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 宏函数
        printf("TAG L:\n");
        af_Code *bt1 = makeElementCode("object", 0, 1, NULL);

        af_Code *bt3 = makeElementCode("func2", 0, 1, NULL);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt1, bt5);

        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }


    {  // 尾调递归优化
        printf("TAG B:\n");
        af_Code *bt1 = makeElementCode("data", ',', 0, "Unknow");
        af_Code *bt2 = makeElementCode("object", 0, 1, NULL);
        connectCode(&bt1, bt2);

        af_Code *bt3 = makeElementCode("func", 0, 1, NULL);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 尾调递归优化2
        printf("TAG C:\n");
        af_Code *bt1 = makeElementCode("data", ',', 0, "Unknow");

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 测试类前缀调用
        printf("TAG D:\n");
        af_Code *bt1 = makeElementCode("data", ',', 0, "Unknow");
        af_Code *bt2 = makeElementCode("func", 0, 1, NULL);
        connectCode(&bt1, bt2);

        af_Code *bt5 = makeBlockCode(parentheses, NULL, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);


        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 测试错误 (无函数指定)
        printf("TAG F: ERROR\n");
        af_Code *bt1 = makeElementCode("data", ',', 0, "Unknow");

        af_Code *bt5 = makeBlockCode(curly, NULL, 0, 1, NULL, NULL);
        connectCode(&bt1, bt5);

        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 测试错误 (object2 Var not found)
        printf("TAG G: ERROR\n");
        af_Code *bt1 = makeElementCode("data", ',', 0, "Unknow");
        af_Code *bt2 = makeElementCode("object2", 0, 1, NULL);

        connectCode(&bt1, bt2);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 测试顺序执行 '(xxx)
        printf("TAG H:\n");
        af_Code *bt3 = makeElementCode("data2", 0, 0, NULL);
        af_Code *bt4 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(parentheses, bt3, '\'', 1, NULL, NULL);

        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt5, env);
        freeAllCode(bt5);
        printf("\n");
    }

    {  // 测试顺序执行 ,[xxx]
        printf("TAG I:\n");
        af_Code *bt3 = makeElementCode("data2", 0, 0, NULL);
        af_Code *bt4 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(brackets, bt3, ',', 1, NULL, NULL);

        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt5, env);
        freeAllCode(bt5);
        printf("\n");
    }

    {  // 测试顺序执行 '(xxx) 【尾调递归优化】
        printf("TAG J:\n");
        af_Code *bt3 = makeElementCode("data2", 0, 0, NULL);
        af_Code *bt4 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(parentheses, bt3, '\'', 1, NULL, NULL);

        iterCode(bt5, env);
        freeAllCode(bt5);
        printf("\n");
    }

    {  // 测试顺序执行 ,[xxx] 【尾调递归优化】
        printf("TAG K:\n");
        af_Code *bt3 = makeElementCode("data2", 0, 0, NULL);
        af_Code *bt4 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(brackets, bt3, ',', 1, NULL, NULL);

        iterCode(bt5, env);
        freeAllCode(bt5);
        printf("\n");
    }

    {  // 双层尾调递归优化 （函数内调用函数）
        printf("TAG M:\n");
        af_Code *bt2 = makeElementCode("func3", 0, 1, NULL);
        af_Code *bt3 = makeBlockCode(curly, bt2, 0, 1, NULL, NULL);

        af_Code *bt4 = makeElementCode("func3", 0, 1, NULL);
        af_Code *bt5 = makeBlockCode(curly, bt4, 0, 1, NULL, NULL);
        connectCode(&bt3, bt5);

        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt3, env);
        freeAllCode(bt3);
        printf("\n");
    }

    {  // 对象函数的调用
        printf("TAG N:\n");
        af_Code *bt1 = makeElementCode("func4", 0, 1, NULL);
        af_Code *bt2 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt1, bt2);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 变量引用调用
        printf("TAG O:\n");
        af_Code *bt1 = makeElementCode("func4", '\'', 1, NULL);
        af_Code *bt2 = makeElementCode("global", 0, 1, NULL);
        connectCode(&bt1, bt2);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 对象函数的调用 (尾调递归优化)
        printf("TAG P:\n");
        af_Code *bt1 = makeElementCode("func4", 0, 1, NULL);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 函数调用
        printf("TAG P:\n");

        af_Code *bt2 = makeElementCode("func", 0, 1, NULL);
        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, NULL, NULL);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // gc测试
        printf("TAG Q:\n");

        af_Code *bt2 = makeElementCode("func5", 0, 1, NULL);
        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, NULL, NULL);
        af_Code *bt3 = makeElementCode("global", 0, 1, NULL);
        af_Code *bt4 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt1, bt3);
        connectCode(&bt3, bt4);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // func_body_dynamic 测试
        printf("TAG R:\n");

        af_Code *bt2 = makeElementCode("func7", 0, 1, NULL);
        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, NULL, NULL);
        af_Code *bt3 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt1, bt3);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 中缀调用测试
        printf("TAG S:\n");

        af_Code *bt2 = makeElementCode("func8", 0, 1, NULL);
        af_Code *bt1 = makeBlockCode(brackets, bt2, 0, 1, NULL, NULL);
        af_Code *bt3 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt1, bt3);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 中缀调用测试
        printf("TAG T: ERROR\n");

        af_Code *bt2 = makeElementCode("func", 0, 1, NULL);
        af_Code *bt1 = makeBlockCode(brackets, bt2, 0, 1, NULL, NULL);
        af_Code *bt3 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt1, bt3);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 中缀保护测试
        printf("TAG U: ERROR\n");

        af_Code *bt2 = makeElementCode("func8", 0, 1, NULL);
        af_Code *bt1 = makeElementCode("global", 0, 1, NULL);

        connectCode(&bt1, bt2);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    printf("freeEnvironment:\n");
    freeEnvironment(env);
    return 0;
}