#include <stdio.h>
#include "aFun.h"

size_t getSize(char *id, af_Object *obj) {
    return sizeof(int *);
}

void initData(char *id, af_Object *obj, int **data, af_Environment *env) {
    *data = calloc(1, sizeof(int));
    **data = 100;
}

void freeData(char *id, af_Object *obj, int **data, af_Environment *env) {
    printf("freeData(): **data = %d\n", **data);
    free(*data);
}

af_VarSpace *getShareVS(char *id, af_Object *obj) {
    return *(af_VarSpace **)getObjectData(obj);
}


size_t getSize_Normal(char *id, af_Object *obj) {
    return sizeof(af_VarSpaceListNode *);
}

void initData_Normal(char *id, af_Object *obj, af_VarSpaceListNode **data, af_Environment *env) {
    *data = makeVarSpaceList(getProtectVarSpace(env));
    printf("initData_Normal(): VarSpace %p\n", *data);
}

void freeData_Normal(char *id, af_Object *obj, af_VarSpaceListNode **data, af_Environment *env) {
    printf("freeData_Normal(): vsl = %p\n", *data);
    freeAllVarSpaceList(*data);
}

void literalSet_Data(char *id, af_Object *obj, void *data, char *str, af_Environment *env) {
    printf("literalSet_Data(): str = %s\n", str);
}

af_FuncBody *testFunc_Normal(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    printf("testFunc_Normal(): I am testFunc_Normal\n");
    af_Object *obj;

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) literal_set = MAKE_SYMBOL(literalSet_Data, objectAPIFunc);
        if (addAPI(literal_set, "obj_literalSetting", api) != 1)
            return NULL;
        obj = makeObject("func", true, api, true, NULL, true, NULL, env);
        FREE_SYMBOL(literal_set);
    }

    pushMessageDown(makeNORMALMessage(obj, env), env);
    return NULL;
}

bool getInfo_Normal(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknown"), true, NULL, *fi);

    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc_Normal, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

bool getAcl_Normal(char *id, af_Object *obj, af_ArgCodeList **acl, af_Code *code, int **mark, af_Environment *env) {
    *acl = makeArgCodeList(makeElementCode("object", NUL, 0, "Unknown"), 0, true, false);
    *mark = calloc(1, sizeof(int));
    **mark = 100;
    return true;
}

bool getVsl_Normal(char *id, af_Object *obj, af_VarSpaceListNode **vsl, void *mark, af_Environment *env) {
    *vsl = *(af_VarSpaceListNode **)getObjectData(obj);
    return true;
}

af_GcList *getGcList_Normal(char *id, af_Object *obj, void *data) {
    af_GcList *gl = pushGcList(glt_vsl, *(af_VarSpaceListNode **)data, NULL);
    return gl;
}

bool getAl_Normal(char *id, af_Object *obj, af_ArgList **al, af_ArgCodeList *acl, void *mark, af_Environment *env) {
    *al = makeArgListFromArgCodeList("test",acl, env);
    return true;
}

void freeMark_Normal(char *id, af_Object *obj, int *mark) {
    printf("freeMark_Normal(): mark = %d\n", *mark);
    free(mark);
}

af_FuncBody *testFuncMacro(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    printf("testFuncMacro(): I am testFuncMacro\n");
    af_Object *obj;

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info = MAKE_SYMBOL(getInfo_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
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

        obj = makeObject("func", true, api, true, NULL, true, NULL, env);
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

    pushMessageDown(makeNORMALMessage(obj, env), env);
    return NULL;
}

bool getInfo_Macro(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, true, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknown"), true, NULL, *fi);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFuncMacro, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

bool getInfo_Tail(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("data3", NUL, 0, "Unknown"), true, NULL, *fi);
    return true;
}

af_FuncBody *testFunc_Obj(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    printf("testFunc_Obj(): I am testFunc_Obj\n");
    af_Object *obj;

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) literal_set = MAKE_SYMBOL(literalSet_Data, objectAPIFunc);
        if (addAPI(literal_set, "obj_literalSetting", api) != 1)
            return NULL;
        obj = makeObject("func", true, api, true, NULL, true, NULL, env);
        FREE_SYMBOL(literal_set);
    }

    pushMessageDown(makeNORMALMessage(obj, env), env);
    return NULL;
}

bool getInfo_Obj(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc_Obj, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

bool isObjTrue(char *id, af_Object *obj) {
    return true;
}

size_t getSize3(char *id, af_Object *obj) {
    return sizeof(af_VarSpace *);
}

void initData3(char *id, af_Object *obj, af_VarSpace **data, af_Environment *env) {
    *data = makeVarSpace(obj, 3, 2, 0, env);
}

void freeData3(char *id, af_Object *obj, af_VarSpace **data, af_Environment *env) {
    if (*data != NULL)
        gc_delReference(*data, env);
    printf("freeData(): *data = %p\n", *data);
}

af_GcList *getGcList3(char *id, af_Object *obj, void *data) {
    af_GcList *gl = pushGcList(glt_vs, *(af_VarSpace **)data, NULL);
    return gl;
}

af_FuncBody *testFunc_Dynamic(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_FuncBody *fb;
    obj = makeObject("obj", true, makeObjectAPI(), true, NULL, true, NULL, env);
    pushMessageDown(makeNORMALMessage(obj, env), env);
    printf("testFunc_Dynamic(%p): I am testFunc_Dynamic\n", obj);

    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc_Dynamic, callFuncBody);
    fb = makeCFuncBody(func1, NULL);
    FREE_SYMBOL(func1);

    return fb;
}

bool getInfo_Dynamic(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc_Dynamic, callFuncBody);
    makeCFuncBodyToFuncInfo(func1, NULL, *fi);
    FREE_SYMBOL(func1);

    makeDynamicFuncBodyToFuncInfo(*fi);
    return true;
}

af_FuncBody *testFunc_GcDestruct_b(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    obj = makeObject("obj", true, makeObjectAPI(), true, NULL, true, NULL, env);
    pushMessageDown(makeNORMALMessage(obj, env), env);
    printf("testFunc_GcDestruct_b(%p): I am testFunc_GcDestruct_b\n", obj);
    fflush(stdout);
    return NULL;
}

af_FuncBody *testFunc_GcDestruct_c(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    obj = makeObject("func", true, makeObjectAPI(), true, NULL, true, NULL, env);
    pushMessageDown(makeNORMALMessage(obj, env), env);
    printf("testFunc_GcDestruct_c[des](%p): I am testFunc_GcDestruct_c\n", obj);
    fflush(stdout);
    return NULL;
}

bool getInfo_GcDestruct_2(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc_GcDestruct_c, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

af_FuncBody *testFunc_GcDestruct_a(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_Object *des;
    obj = makeObject("func", true, makeObjectAPI(), true, NULL, true, NULL, env);

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info7 = MAKE_SYMBOL(getInfo_GcDestruct_2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
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
        des = makeObject("func-des", true, api, true, NULL, true, NULL, env);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_info7);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
    }

    setObjectAttributes(mg_gc_destruct, 3, 3, 3, des, obj, obj, env);
    pushMessageDown(makeNORMALMessage(obj, env), env);
    printf("testFunc_GcDestruct_a[des](%p, %p): I am testFunc_GcDestruct_a\n", obj, des);
    gc_delReference(des, env);  // obj不需要 delReference, 因为他存在于NORMAL_Message中
    return NULL;
}

bool getInfo_GcDestruct(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc_GcDestruct_a, callFuncBody);
    makeCFuncBodyToFuncInfo(func1, NULL, *fi);
    FREE_SYMBOL(func1);

    DLC_SYMBOL(callFuncBody) func2 = MAKE_SYMBOL(testFunc_GcDestruct_b, callFuncBody);
    makeCFuncBodyToFuncInfo(func2, NULL, *fi);
    FREE_SYMBOL(func2);
    return true;
}

af_FuncBody *testFunc_Gc(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_Object *des;
    obj = makeObject("func", true, makeObjectAPI(), true, NULL, true, NULL, env);

    {
        af_ObjectAPI *api = makeObjectAPI();
        /* 无get_acl API 表示析构函数没有参数 */
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info6 = MAKE_SYMBOL(getInfo_GcDestruct, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
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
        des = makeObject("func-des", true, api, true, NULL, true, NULL, env);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_info6);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
    }

    setObjectAttributes(mg_gc_destruct, 3, 3, 3, des, obj, obj, env);
    pushMessageDown(makeNORMALMessage(obj, env), env);
    printf("testFunc_Gc(%p, %p): I am testFunc_Gc\n", obj, des);
    gc_delReference(des, env);
    return NULL;
}

bool getInfo_Gc(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc_Gc, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknown"), true, NULL, *fi);
    return true;
}

bool getInfo_NotVar(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, true, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("no-var", NUL, 1, "func9.info.aun"), true, NULL, *fi);
    return true;
}

bool getInfo_Import(char *id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    makeImportFuncBodyToFuncInfo(makeElementCode("global", NUL, 1, "func9.info.aun"), true, NULL, *fi);
    return true;
}

bool isInfixTrue(char *id, af_Object *obj) {
    return true;
}

struct GDData {
    af_Object *func;
    bool no_first;
};

af_GuardianList *gd_func(char *type, bool is_guard, struct GDData *data, af_Environment *env) {
    printf("gd_func:run-Guardian-func\n");

    if (data->no_first)
        return NULL;

    af_GuardianList *gd = NULL;
    data->no_first = true;
    gc_addReference(data->func, env);  // data->func 本身有一次gc引用, 此次再使用一次gc引用, gd_destruct和freeGuardianList时各释放一次

    pushGuardianList(NULL, data->func, &gd, env);
    return gd;
}

void gd_destruct(char *type, struct GDData *data, af_Environment *env) {
    gc_delReference(data->func, env);
}

int main(int argc, char **argv) {
    jmp_buf main_buf;
    char *base_path = getExedir(1);
    if (base_path == NULL)
        goto INIT_ERROR;

    if (setjmp(main_buf) == 1)
        return EXIT_FAILURE;

    aFunInitInfo info = {
            .base_dir=base_path,
            .level=log_track,
            .log_asyn=true,
            .buf=&main_buf
    };

    if (!aFunInit(&info)) {
INIT_ERROR:
        free(base_path);
        printf_stderr(0, "aFunlang init error\n");
        return EXIT_FAILURE;
    } else
        free(base_path);


    af_Environment *env = creatAFunEnvironment(0, NULL);
    aFunRunInfo ri = {.signal=true};
    defineRunEnv(&ri);

    if(!pushLiteralRegex("data.*", "func-normal", true, env)) {
        fprintf(stderr, "pushLiteralRegex Error\n");
        goto RETURN_1;
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
            goto RETURN_2;
        if (addAPI(initData_3, "obj_initData", api) != 1)
            goto RETURN_2;
        if (addAPI(freeData_3, "obj_destructData", api) != 1)
            goto RETURN_2;
        if (addAPI(getShareVS_, "obj_getShareVarSpace", api) != 1)
            goto RETURN_2;
        if (addAPI(get_gl3, "obj_getGcList", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("object", 3, 3, 3,
                                        (obj = makeObject("object", true, api, true, NULL, true, NULL, env)),
                                        env);
        FREE_SYMBOL(getSize_3);
        FREE_SYMBOL(initData_3);
        FREE_SYMBOL(freeData_3);
        FREE_SYMBOL(getShareVS_);
        FREE_SYMBOL(get_gl3);
        printf("object(%p)\n", obj);
        gc_delReference(obj, env);
    }

    af_Object *af_func = NULL;

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info = MAKE_SYMBOL(getInfo_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            goto RETURN_2;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            goto RETURN_2;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            goto RETURN_2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_info, "obj_funcGetInfo", api) != 1)
            goto RETURN_2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            goto RETURN_2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("func-normal", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, NULL, true, NULL, env)), env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        af_func = obj;
        gc_delReference(obj, env);
        printf("func-normal(%p)\n", obj);
    }

//    {
//        af_ObjectAPI *api = makeObjectAPI();
//        af_Object *obj;
//        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_info2 = MAKE_SYMBOL(getInfo_Macro, objectAPIFunc);  // 宏函数
//        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
//        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
//            goto RETURN_2;
//        if (addAPI(initData_2, "obj_initData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(freeData_2, "obj_destructData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_gl, "obj_getGcList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_info2, "obj_funcGetInfo", api) != 1)
//            goto RETURN_2;
//        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
//            goto RETURN_2;
//
//        makeVarToProtectVarSpace("macro", 3, 3, 3,
//                                        (obj = makeObject("func", true, api, true, NULL, true, NULL, env)), env);
//        FREE_SYMBOL(get_alc);
//        FREE_SYMBOL(get_vsl);
//        FREE_SYMBOL(get_al);
//        FREE_SYMBOL(get_info2);
//        FREE_SYMBOL(free_mark);
//        FREE_SYMBOL(get_gl);
//        FREE_SYMBOL(getSize_2);
//        FREE_SYMBOL(initData_2);
//        FREE_SYMBOL(freeData_2);
//        printf("macro(%p)\n", obj);
//        gc_delReference(obj, env);
//    }
//
//    {
//        af_ObjectAPI *api = makeObjectAPI();
//        af_Object *obj;
//        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_info3 = MAKE_SYMBOL(getInfo_Tail, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
//        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
//            goto RETURN_2;
//        if (addAPI(initData_2, "obj_initData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(freeData_2, "obj_destructData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_gl, "obj_getGcList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_info3, "obj_funcGetInfo", api) != 1)
//            goto RETURN_2;
//        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
//            goto RETURN_2;
//
//        makeVarToProtectVarSpace("func-tail", 3, 3, 3,
//                                        (obj = makeObject("func", true, api, true, NULL, true, NULL, env)), env);
//        FREE_SYMBOL(get_alc);
//        FREE_SYMBOL(get_vsl);
//        FREE_SYMBOL(get_al);
//        FREE_SYMBOL(get_info3);
//        FREE_SYMBOL(free_mark);
//        FREE_SYMBOL(get_gl);
//        FREE_SYMBOL(getSize_2);
//        FREE_SYMBOL(initData_2);
//        FREE_SYMBOL(freeData_2);
//        printf("func-tail(%p)\n", obj);
//        gc_delReference(obj, env);
//    }
//
//    {
//        af_ObjectAPI *api = makeObjectAPI();
//        af_Object *obj;
//        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_info4 = MAKE_SYMBOL(getInfo_Obj, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) obj_func = MAKE_SYMBOL(isObjTrue, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
//        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
//            goto RETURN_2;
//        if (addAPI(initData_2, "obj_initData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(freeData_2, "obj_destructData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_gl, "obj_getGcList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_info4, "obj_funcGetInfo", api) != 1)
//            goto RETURN_2;
//        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
//            goto RETURN_2;
//        if (addAPI(obj_func, "obj_isObjFunc", api) != 1)
//            goto RETURN_2;
//
//        makeVarToProtectVarSpace("func-obj", 3, 3, 3,
//                                        (obj = makeObject("func", true, api, true, NULL, true, NULL, env)), env);
//        FREE_SYMBOL(get_alc);
//        FREE_SYMBOL(get_vsl);
//        FREE_SYMBOL(get_al);
//        FREE_SYMBOL(get_info4);
//        FREE_SYMBOL(free_mark);
//        FREE_SYMBOL(obj_func);
//        FREE_SYMBOL(get_gl);
//        FREE_SYMBOL(getSize_2);
//        FREE_SYMBOL(initData_2);
//        FREE_SYMBOL(freeData_2);
//        printf("func-obj(%p)\n", obj);
//        gc_delReference(obj, env);
//    }
//
//    {
//        af_ObjectAPI *api = makeObjectAPI();
//        af_Object *obj;
//        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_info5 = MAKE_SYMBOL(getInfo_Gc, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
//        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
//            goto RETURN_2;
//        if (addAPI(initData_2, "obj_initData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(freeData_2, "obj_destructData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_info5, "obj_funcGetInfo", api) != 1)
//            goto RETURN_2;
//        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_gl, "obj_getGcList", api) != 1)
//            goto RETURN_2;
//
//        makeVarToProtectVarSpace("func-gc", 3, 3, 3,
//                                        (obj = makeObject("func", true, api, true, NULL, true, NULL, env)), env);
//        FREE_SYMBOL(get_alc);
//        FREE_SYMBOL(get_vsl);
//        FREE_SYMBOL(get_al);
//        FREE_SYMBOL(get_info5);
//        FREE_SYMBOL(free_mark);
//        FREE_SYMBOL(get_gl);
//        FREE_SYMBOL(getSize_2);
//        FREE_SYMBOL(initData_2);
//        FREE_SYMBOL(freeData_2);
//        printf("func-gc(%p)\n", obj);
//        gc_delReference(obj, env);
//    }
//
//    {
//        af_ObjectAPI *api = makeObjectAPI();
//        af_Object *obj;
//        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_info9 = MAKE_SYMBOL(getInfo_Dynamic, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
//        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
//            goto RETURN_2;
//        if (addAPI(initData_2, "obj_initData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(freeData_2, "obj_destructData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_info9, "obj_funcGetInfo", api) != 1)
//            goto RETURN_2;
//        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_gl, "obj_getGcList", api) != 1)
//            goto RETURN_2;
//
//        makeVarToProtectVarSpace("func-body-dynamic", 3, 3, 3,
//                                        (obj = makeObject("func", true, api, true, NULL, true, NULL, env)), env);
//        FREE_SYMBOL(get_alc);
//        FREE_SYMBOL(get_vsl);
//        FREE_SYMBOL(get_al);
//        FREE_SYMBOL(get_info9);
//        FREE_SYMBOL(free_mark);
//        FREE_SYMBOL(get_gl);
//        FREE_SYMBOL(getSize_2);
//        FREE_SYMBOL(initData_2);
//        FREE_SYMBOL(freeData_2);
//        printf("func-body-dynamic(%p)\n", obj);
//        gc_delReference(obj, env);
//    }
//
//    {
//        af_ObjectAPI *api = makeObjectAPI();
//        af_Object *obj;
//        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_info = MAKE_SYMBOL(getInfo_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) infix_func = MAKE_SYMBOL(isInfixTrue, objectAPIFunc);
//        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
//            goto RETURN_2;
//        if (addAPI(initData_2, "obj_initData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(freeData_2, "obj_destructData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_info, "obj_funcGetInfo", api) != 1)
//            goto RETURN_2;
//        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_gl, "obj_getGcList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(infix_func, "obj_isInfixFunc", api) != 1)
//            goto RETURN_2;
//
//        makeVarToProtectVarSpace("func-brackets", 3, 3, 3,
//                                        (obj = makeObject("func", true, api, true, NULL, true, NULL, env)), env);
//        FREE_SYMBOL(get_alc);
//        FREE_SYMBOL(get_vsl);
//        FREE_SYMBOL(get_al);
//        FREE_SYMBOL(get_info);
//        FREE_SYMBOL(free_mark);
//        FREE_SYMBOL(get_gl);
//        FREE_SYMBOL(getSize_2);
//        FREE_SYMBOL(initData_2);
//        FREE_SYMBOL(freeData_2);
//        FREE_SYMBOL(infix_func);
//        printf("func-brackets(%p)\n", obj);
//        gc_delReference(obj, env);
//    }
//
//    {
//        af_ObjectAPI *api = makeObjectAPI();
//        af_Object *obj;
//        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_info10 = MAKE_SYMBOL(getInfo_NotVar, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
//        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
//            goto RETURN_2;
//        if (addAPI(initData_2, "obj_initData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(freeData_2, "obj_destructData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_info10, "obj_funcGetInfo", api) != 1)
//            goto RETURN_2;
//        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_gl, "obj_getGcList", api) != 1)
//            goto RETURN_2;
//
//        makeVarToProtectVarSpace("func-no-var", 3, 3, 3,
//                                        (obj = makeObject("func", true, api, true, NULL, true, NULL, env)), env);
//        FREE_SYMBOL(get_alc);
//        FREE_SYMBOL(get_vsl);
//        FREE_SYMBOL(get_al);
//        FREE_SYMBOL(get_info10);
//        FREE_SYMBOL(free_mark);
//        FREE_SYMBOL(get_gl);
//        FREE_SYMBOL(getSize_2);
//        FREE_SYMBOL(initData_2);
//        FREE_SYMBOL(freeData_2);
//        printf("func-no-var(%p)\n", obj);
//        gc_delReference(obj, env);
//    }
//
//    {
//        af_ObjectAPI *api = makeObjectAPI();
//        af_Object *obj;
//        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_info11 = MAKE_SYMBOL(getInfo_Import, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
//        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
//        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
//            goto RETURN_2;
//        if (addAPI(initData_2, "obj_initData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(freeData_2, "obj_destructData", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_info11, "obj_funcGetInfo", api) != 1)
//            goto RETURN_2;
//        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
//            goto RETURN_2;
//        if (addAPI(get_gl, "obj_getGcList", api) != 1)
//            goto RETURN_2;
//
//        makeVarToProtectVarSpace("func-import", 3, 3, 3,
//                                        (obj =makeObject("func", true, api, true, NULL, true, NULL, env)), env);
//        FREE_SYMBOL(get_alc);
//        FREE_SYMBOL(get_vsl);
//        FREE_SYMBOL(get_al);
//        FREE_SYMBOL(get_info11);
//        FREE_SYMBOL(free_mark);
//        FREE_SYMBOL(get_gl);
//        FREE_SYMBOL(getSize_2);
//        FREE_SYMBOL(initData_2);
//        FREE_SYMBOL(freeData_2);
//        printf("func-import(%p)\n", obj);
//        gc_delReference(obj, env);
//    }

    {
        af_Object *obj = getGlobal(env);
        printf("global(%p)\n", obj);
        gc_delReference(obj, env);
    }
    printf("\n");
    sleep(2);

    {  // 正常程序
        printf("TAG A: [Normal test]\n");
        af_Code *bt1 = makeElementCode("object", 0, 1, "Taga.aun");
        af_Code *bt2 = makeElementCode("data", 0, 0, NULL);
        pushCode(&bt1, bt2);

        af_Code *bt3 = makeElementCode("func-normal", 0, 1, NULL);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        pushCode(&bt1, bt5);

        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
        pushCode(&bt5, bt6);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
        printf("\n");
    }

    sleep(2);

//    {  // 宏函数
//        printf("TAG L: [Macro test]\n");
//        af_Code *bt1 = makeElementCode("object", 0, 1, "Tagl.aun");
//
//        af_Code *bt3 = makeElementCode("macro", 0, 1, NULL);
//        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
//        pushCode(&bt1, bt5);
//
//        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
//        pushCode(&bt5, bt6);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 测试顺序执行 '(xxx)
//        printf("TAG E: [ex]\n");
//        af_Code *bt3 = makeElementCode("data2", 0, 0, NULL);
//        af_Code *bt4 = makeElementCode("global", 0, 1, NULL);
//
//        pushCode(&bt3, bt4);
//
//        af_Code *bt5 = makeBlockCode(parentheses, bt3, '\'', 1, "Tage.aun", NULL);
//
//        runCodeFromMemory(bt5, 0, env);
//        freeAllCode(bt5);
//        printf("\n");
//    }
//
//    {  // 尾调用优化
//        printf("TAG B: [Tail test]\n");
//        af_Code *bt1 = makeElementCode("object", 0, 1, NULL);
//
//        af_Code *bt2 = makeElementCode("func-normal", 0, 1, NULL);
//        af_Code *bt3 = makeBlockCode(curly, bt2, 0, 1, NULL, NULL);
//        pushCode(&bt1, bt3);
//
//        af_Code *bt4 = makeBlockCode(parentheses, bt1, '\'', 1, "Tagb.aun", NULL);
//
//        runCodeFromMemory(bt4, 0, env);
//        freeAllCode(bt4);
//        printf("\n");
//    }
//
//    {  // 尾调用优化2
//        printf("TAG C: [Tail test on literal]\n");
//        af_Code *bt1 = makeElementCode("data", 0, 0, "Tagc.aun");
//        af_Code *bt2 = makeBlockCode(parentheses, bt1, '\'', 1, "Tagc.aun", NULL);
//
//        runCodeFromMemory(bt2, 0, env);
//        freeAllCode(bt2);
//        printf("\n");
//    }
//
//    {  // 测试类前缀调用
//        printf("TAG D: [parentheses-call]\n");
//        af_Code *bt1 = makeElementCode("func-normal", 0, 1, "Tagd.aun");
//        af_Code *bt2 = makeBlockCode(parentheses, NULL, 0, 1, NULL, NULL);
//        pushCode(&bt1, bt2);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//
//    {  // 测试顺序执行 $[xxx]
//        printf("TAG F: [ex-first]\n");
//        af_Code *bt3 = makeElementCode("data2", 0, 0, NULL);
//        af_Code *bt4 = makeElementCode("global", 0, 1, NULL);
//
//        pushCode(&bt3, bt4);
//
//        af_Code *bt5 = makeBlockCode(brackets, bt3, '$', 1, "Tagf.aun", NULL);
//
//        runCodeFromMemory(bt5, 0, env);
//        freeAllCode(bt5);
//        printf("\n");
//    }
//
//
//    {  // 双层尾调用优化 （函数内调用函数）
//        printf("TAG I: [Double tail test]\n");
//        af_Code *bt1 = makeElementCode("func-tail", 0, 1, NULL);
//        af_Code *bt2 = makeBlockCode(curly, bt1, 0, 1, "Tagi.aun", NULL);
//        af_Code *bt3 = makeBlockCode(parentheses, bt2, '\'', 1, "Tagi.aun", NULL);
//
//        runCodeFromMemory(bt3, 0, env);
//        freeAllCode(bt3);
//        printf("\n");
//    }
//
//    {  // 对象函数的调用
//        printf("TAG J: [object]\n");
//        af_Code *bt1 = makeElementCode("func-obj", 0, 1, "Tagj.aun");
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 变量引用调用
//        printf("TAG K: [not object]\n");
//        af_Code *bt1 = makeElementCode("func-obj", '\'', 1, "Tagk.aun");
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 对象函数的调用 (尾调用优化)
//        printf("TAG L: [pbject-tail]\n");
//        af_Code *bt1 = makeElementCode("func-obj", 0, 1, "Tagl.aun");
//        af_Code *bt2 = makeBlockCode(parentheses, bt1, '\'', 1, "Tagi.aun", NULL);
//
//        runCodeFromMemory(bt2, 0, env);
//        freeAllCode(bt2);
//        printf("\n");
//    }
//
//
//    {  // gc测试
//        printf("TAG N: [gc-test]\n");
//
//        af_Code *bt2 = makeElementCode("func-gc", 0, 1, NULL);
//        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, "Tagn.aun", NULL);
//        af_Code *bt3 = makeElementCode("global", 0, 1, NULL);
//        af_Code *bt4 = makeElementCode("global", 0, 1, NULL);
//
//        pushCode(&bt1, bt3);
//        pushCode(&bt3, bt4);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // func_body_dynamic 测试
//        printf("TAG O: [func body dynamic]\n");
//
//        af_Code *bt2 = makeElementCode("func-body-dynamic", 0, 1, NULL);
//        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, "Tago.aun", NULL);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 中缀调用测试
//        printf("TAG P: [infix-call]\n");
//
//        af_Code *bt2 = makeElementCode("func-brackets", 0, 1, NULL);
//        af_Code *bt1 = makeBlockCode(brackets, bt2, 0, 1, "Tagp.aun", NULL);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // func_body_import 测试
//        printf("TAG Q: [import]\n");
//
//        af_Code *bt2 = makeElementCode("func-import", 0, 1, NULL);
//        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, "Tagq.aun", NULL);
//        af_Code *bt3 = makeElementCode("global", 0, 1, NULL);
//
//        pushCode(&bt1, bt3);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 导入式运行
//        printf("TAG R: [import-Tagr]\n");
//        af_Code *bt1 = makeElementCode("object", 0, 1, "Tagr.aun");
//        af_Code *bt2 = makeElementCode("data", 0, 0, NULL);
//        pushCode(&bt1, bt2);
//
//        af_Code *bt3 = makeElementCode("func-normal", 0, 1, NULL);
//        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
//        pushCode(&bt2, bt5);
//
//        af_Code *bt6 = makeElementCode("global", 0, 1, NULL);
//        pushCode(&bt5, bt6);
//
//        runCodeFromMemory(bt1, 1, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {
//        printf("TAG S: [string]\n");
//        int exit_code = runCodeFromString("str\ndata\n{func-normal}\nglobal\n", "Tags-string.aun", 1, env);
//        printf("exit code = %d\n\n", exit_code);
//    }
//
//
//    {
//        DLC_SYMBOL(GuardianFunc) func = MAKE_SYMBOL(gd_func, GuardianFunc);
//        DLC_SYMBOL(GuardianDestruct) des = MAKE_SYMBOL(gd_destruct, GuardianDestruct);
//        struct GDData *data = NULL;
//        addGuardian("test", false, true, sizeof(struct GDData), func, des, (void **) &data, env);
//        data->func = af_func;
//        gc_addReference(af_func, env);
//        FREE_SYMBOL(func);
//        FREE_SYMBOL(des);
//
//        printf("TAG U: [guardian]\n");
//        af_Code *bt1 = makeElementCode("func-normal", 0, 1, "TagU.aun");
//        af_Code *bt2 = makeElementCode("global", 0, 1, "TagU.aun");
//        af_Code *bt3 = makeElementCode("global", 0, 1, "TagU.aun");
//
//        pushCode(&bt1, bt2);
//        pushCode(&bt2, bt3);
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//
//        bool re = popGuardian("test", env);
//        printf("popGuardian: %d\n\n", re);
//    }
//
//    {
//        printf("TAG V: [Thread]\n");
//        af_Code *bt1 = makeElementCode("object", 0, 1, "Tagv.aun");
//
//        startRunThread(env, NULL, bt1, false, true, true, true, true);
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    /* 错误用例 */
//
//    {  // 中缀调用测试
//        printf("TAG a: ERROR\n");
//
//        af_Code *bt2 = makeElementCode("func-normal", 0, 1, NULL);
//        af_Code *bt1 = makeBlockCode(brackets, bt2, 0, 1, "Taga-error.aun", NULL);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 测试错误 (无函数指定)
//        printf("TAG b: ERROR\n");
//        af_Code *bt1 = makeBlockCode(curly, NULL, 0, 1, "Tagb-error.aun", NULL);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 测试错误 (object2 Var not found)
//        printf("TAG c: ERROR\n");
//        af_Code *bt1 = makeElementCode("object2", 0, 1, "Tagc-error.aun");
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 中缀保护测试
//        printf("TAG d: ERROR\n");
//
//        af_Code *bt1 = makeElementCode("global", 0, 1, "Tagd-error.aun");
//        af_Code *bt2 = makeElementCode("func-brackets", 0, 2, NULL);
//
//        pushCode(&bt1, bt2);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }
//
//    {  // 错误回溯测试
//        printf("TAG e: ERROR\n");
//
//        af_Code *bt2 = makeElementCode("func-no-var", 0, 1, NULL);
//        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, "Tage-error.aun", NULL);
//
//        af_Code *bt3 = makeElementCode("global", 0, 1, NULL);
//        pushCode(&bt1, bt3);
//
//        runCodeFromMemory(bt1, 0, env);
//        freeAllCode(bt1);
//        printf("\n");
//    }

    printf("freeEnvironment:\n");
    destructAFunEnvironment(env);

    undefRunEnv(&ri);
    printf("Exit at 0.\n");
#ifndef IN_CTEST
    getc(stdin);
#endif
    aFunDestruct();
    aFunExit(0);

RETURN_1:
    undefRunEnv(&ri);
    printf("Exit at 1.\n");
#ifndef IN_CTEST
    getc(stdin);
#endif
    aFunDestruct();
    aFunExit(1);

RETURN_2:
    undefRunEnv(&ri);
    printf("Exit at 2.\n");
#ifndef IN_CTEST
    getc(stdin);
#endif
    aFunDestruct();
    aFunExit(2);
}