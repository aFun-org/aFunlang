#include <cstdio>
#include "aFun.hpp"

size_t getSize(const std::string &id, af_Object *obj) {
    return sizeof(int *);
}

void initData(const std::string &id, af_Object *obj, int **data, af_Environment *env) {
    *data = calloc(1, int);
    **data = 100;
}

void freeData(const std::string &id, af_Object *obj, int **data, af_Environment *env) {
    printf("freeData(): **data = %d\n", **data);
    free(*data);
}

af_VarSpace *getShareVS(const std::string &id, af_Object *obj) {
    return *(af_VarSpace **)getObjectData(obj);
}


size_t getSize_Normal(const std::string &id, af_Object *obj) {
    return sizeof(af_VarList *);
}

void initData_Normal(const std::string &id, af_Object *obj, af_VarList **data, af_Environment *env) {
    *data = pushProtectVarList(nullptr, env);
    printf("initData_Normal(): VarSpace %p\n", *data);
}

void freeData_Normal(const std::string &id, af_Object *obj, af_VarList **data, af_Environment *env) {
    printf("freeData_Normal(): vsl = %p\n", *data);
    freeAllVarSpaceList(*data);
}

void literalSet_Data(const std::string &id, af_Object *obj, void *data, char *str, af_Environment *env) {
    printf("literalSet_Data(): str = %s\n", str);
}

af_FuncBody *testFunc_Normal(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    printf("testFunc_Normal(): I am testFunc_Normal\n");
    af_Object *obj;

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) literal_set = MAKE_SYMBOL(literalSet_Data, objectAPIFunc);
        if (addAPI(literal_set, "obj_literalSetting", api) != 1)
            return nullptr;
        obj = makeObject("func", true, api, true, nullptr, true, nullptr, env);
        FREE_SYMBOL(literal_set);
    }

    pushMessageDown(makeNORMALMessage(obj, env), env);
    return nullptr;
}

bool getInfo_Normal(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknown"), true, nullptr, *fi);

    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc_Normal, callFuncBody);
    makeCFuncBodyToFuncInfo(func, nullptr, *fi);
    FREE_SYMBOL(func);
    return true;
}

bool getAcl_Normal(const std::string &id, af_Object *obj, af_ArgCodeList **acl, af_Code *code, int **mark, af_Environment *env) {
    *acl = makeArgCodeList(makeElementCode("object", NUL, 0, "Unknown"), 0, true, false);
    *mark = calloc(1, int);
    **mark = 100;
    return true;
}

bool getVsl_Normal(const std::string &id, af_Object *obj, af_VarList **vsl, void *mark, af_Environment *env) {
    *vsl = *(af_VarList **)getObjectData(obj);
    return true;
}

af_GcList *getGcList_Normal(const std::string &id, af_Object *obj, void *data) {
    af_GcList *gl = pushGcList(glt_vsl, *(af_VarList **)data, nullptr);
    return gl;
}

bool getAl_Normal(const std::string &id, af_Object *obj, af_ArgList **al, af_ArgCodeList *acl, void *mark, af_Environment *env) {
    *al = makeArgListFromArgCodeList("test",acl, env);
    return true;
}

void freeMark_Normal(const std::string &id, af_Object *obj, int *mark) {
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
            return nullptr;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return nullptr;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return nullptr;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            return nullptr;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return nullptr;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            return nullptr;
        if (addAPI(get_info, "obj_funcGetInfo", api) != 1)
            return nullptr;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            return nullptr;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return nullptr;

        obj = makeObject("func", true, api, true, nullptr, true, nullptr, env);
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
    return nullptr;
}

bool getInfo_Macro(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, true, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknown"), true, nullptr, *fi);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFuncMacro, callFuncBody);
    makeCFuncBodyToFuncInfo(func, nullptr, *fi);
    FREE_SYMBOL(func);
    return true;
}

bool getInfo_Tail(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("data3", NUL, 0, "Unknown"), true, nullptr, *fi);
    return true;
}

af_FuncBody *testFunc_Obj(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    printf("testFunc_Obj(): I am testFunc_Obj\n");
    af_Object *obj;

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) literal_set = MAKE_SYMBOL(literalSet_Data, objectAPIFunc);
        if (addAPI(literal_set, "obj_literalSetting", api) != 1)
            return nullptr;
        obj = makeObject("func", true, api, true, nullptr, true, nullptr, env);
        FREE_SYMBOL(literal_set);
    }

    pushMessageDown(makeNORMALMessage(obj, env), env);
    return nullptr;
}

bool getInfo_Obj(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc_Obj, callFuncBody);
    makeCFuncBodyToFuncInfo(func, nullptr, *fi);
    FREE_SYMBOL(func);
    return true;
}

bool isObjTrue(const std::string &id, af_Object *obj) {
    return true;
}

size_t getSize3(const std::string &id, af_Object *obj) {
    return sizeof(af_VarSpace *);
}

void initData3(const std::string &id, af_Object *obj, af_VarSpace **data, af_Environment *env) {
    *data = makeVarSpace(obj, 3, 2, 0, env);
    gc_delVarSpaceReference(*data, env);
}

void freeData3(const std::string &id, af_Object *obj, af_VarSpace **data, af_Environment *env) {
    printf("freeData(): *data = %p\n", *data);
}

af_GcList *getGcList3(const std::string &id, af_Object *obj, void *data) {
    af_GcList *gl = pushGcList(glt_vs, *(af_VarSpace **)data, nullptr);
    return gl;
}

af_FuncBody *testFunc_Dynamic(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_FuncBody *fb;
    obj = makeObject("obj", true, makeObjectAPI(), true, nullptr, true, nullptr, env);
    pushMessageDown(makeNORMALMessage(obj, env), env);
    printf("testFunc_Dynamic(%p): I am testFunc_Dynamic\n", obj);

    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc_Dynamic, callFuncBody);
    fb = makeCFuncBody(func1, nullptr);
    FREE_SYMBOL(func1);

    return fb;
}

bool getInfo_Dynamic(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc_Dynamic, callFuncBody);
    makeCFuncBodyToFuncInfo(func1, nullptr, *fi);
    FREE_SYMBOL(func1);

    makeDynamicFuncBodyToFuncInfo(*fi);
    return true;
}

af_FuncBody *testFunc_GcDestruct_b(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    obj = makeObject("obj", true, makeObjectAPI(), true, nullptr, true, nullptr, env);
    pushMessageDown(makeNORMALMessage(obj, env), env);
    printf("testFunc_GcDestruct_b(%p): I am testFunc_GcDestruct_b\n", obj);
    fflush(stdout);
    return nullptr;
}

af_FuncBody *testFunc_GcDestruct_c(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    obj = makeObject("func", true, makeObjectAPI(), true, nullptr, true, nullptr, env);
    pushMessageDown(makeNORMALMessage(obj, env), env);
    printf("testFunc_GcDestruct_c[des](%p): I am testFunc_GcDestruct_c\n", obj);
    fflush(stdout);
    return nullptr;
}

bool getInfo_GcDestruct_2(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc_GcDestruct_c, callFuncBody);
    makeCFuncBodyToFuncInfo(func, nullptr, *fi);
    FREE_SYMBOL(func);
    return true;
}

af_FuncBody *testFunc_GcDestruct_a(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_Object *des;
    obj = makeObject("func", true, makeObjectAPI(), true, nullptr, true, nullptr, env);

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info7 = MAKE_SYMBOL(getInfo_GcDestruct_2, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_gl = MAKE_SYMBOL(getGcList_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) getSize_2 = MAKE_SYMBOL(getSize_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_2 = MAKE_SYMBOL(initData_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_2 = MAKE_SYMBOL(freeData_Normal, objectAPIFunc);
        if (addAPI(getSize_2, "obj_getDataSize", api) != 1)
            return nullptr;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return nullptr;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return nullptr;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return nullptr;
        if (addAPI(get_info7, "obj_funcGetInfo", api) != 1)
            return nullptr;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return nullptr;
        des = makeObject("func-des", true, api, true, nullptr, true, nullptr, env);
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
    gc_delObjectReference(des, env);  // obj不需要 delReference, 因为他存在于NORMAL_Message中
    return nullptr;
}

bool getInfo_GcDestruct(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func1 = MAKE_SYMBOL(testFunc_GcDestruct_a, callFuncBody);
    makeCFuncBodyToFuncInfo(func1, nullptr, *fi);
    FREE_SYMBOL(func1);

    DLC_SYMBOL(callFuncBody) func2 = MAKE_SYMBOL(testFunc_GcDestruct_b, callFuncBody);
    makeCFuncBodyToFuncInfo(func2, nullptr, *fi);
    FREE_SYMBOL(func2);
    return true;
}

af_FuncBody *testFunc_Gc(af_CallFuncInfo *cfi, af_Environment *env) {  // 测试用函数
    af_Object *obj;
    af_Object *des;
    obj = makeObject("func", true, makeObjectAPI(), true, nullptr, true, nullptr, env);

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
            return nullptr;
        if (addAPI(initData_2, "obj_initData", api) != 1)
            return nullptr;
        if (addAPI(freeData_2, "obj_destructData", api) != 1)
            return nullptr;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            return nullptr;
        if (addAPI(get_info6, "obj_funcGetInfo", api) != 1)
            return nullptr;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            return nullptr;
        des = makeObject("func-des", true, api, true, nullptr, true, nullptr, env);
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
    gc_delObjectReference(des, env);
    return nullptr;
}

bool getInfo_Gc(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc_Gc, callFuncBody);
    makeCFuncBodyToFuncInfo(func, nullptr, *fi);
    FREE_SYMBOL(func);
    makeCodeFuncBodyToFuncInfo(makeElementCode("test", NUL, 0, "Unknown"), true, nullptr, *fi);
    return true;
}

bool getInfo_NotVar(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, true, true, true);
    makeCodeFuncBodyToFuncInfo(makeElementCode("no-var", NUL, 1, "func9.info.aun"), true, nullptr, *fi);
    return true;
}

bool getInfo_Import(const std::string &id, af_Object *obj, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, true, true);
    makeImportFuncBodyToFuncInfo(makeElementCode("global", NUL, 1, "func9.info.aun"), true, nullptr, *fi);
    return true;
}

bool isInfixTrue(const std::string &id, af_Object *obj) {
    return true;
}

struct GDData {
    af_Object *func;
    bool no_first;
};

af_GuardianList *gd_func(char *type, bool is_guard, struct GDData *data, af_Environment *env) {
    printf("gd_func:run-Guardian-func\n");

    if (data->no_first)
        return nullptr;

    af_GuardianList *gd = nullptr;
    paf_GuardianList pgd = &gd;

    data->no_first = true;
    gc_addObjectReference(data->func, env);  // data->func 本身有一次gc引用, 此次再使用一次gc引用, gd_destruct和freeGuardianList时各释放一次
    pushGuardianList(nullptr, data->func, pgd, env);
    return gd;
}

void gd_destruct(char *type, struct GDData *data, af_Environment *env) {
    gc_delObjectReference(data->func, env);
}

int main(int argc, char **argv) {
    jmp_buf main_buf;
    char *base_path = getExedir(1);
    if (base_path == nullptr) {
        free(base_path);
        printf_stderr(0, "aFunlang init error\n");
        return EXIT_FAILURE;
    }

    if (setjmp(main_buf) == 1)
        return EXIT_FAILURE;

    aFunInitInfo info = {
            .base_dir=base_path,
            .log_asyn=true,
            .buf=&main_buf,
            .level=log_track,
    };

    if (!aFunInit(&info)) {
        free(base_path);
        printf_stderr(0, "aFunlang init error\n");
        return EXIT_FAILURE;
    } else
        free(base_path);


    af_Environment *env = creatAFunEnvironment(0, nullptr);
    aFunRunInfo ri = {.signal=true};
    af_Object *af_func = nullptr;
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
                                        (obj = makeObject("object", true, api, true, nullptr, true, nullptr, env)),
                                        env);
        FREE_SYMBOL(getSize_3);
        FREE_SYMBOL(initData_3);
        FREE_SYMBOL(freeData_3);
        FREE_SYMBOL(getShareVS_);
        FREE_SYMBOL(get_gl3);
        printf("object(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

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
                                        (obj = makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
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
        gc_delObjectReference(obj, env);
        printf("func-normal(%p)\n", obj);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info2 = MAKE_SYMBOL(getInfo_Macro, objectAPIFunc);  // 宏函数
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
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_info2, "obj_funcGetInfo", api) != 1)
            goto RETURN_2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("macro", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info2);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("macro(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info3 = MAKE_SYMBOL(getInfo_Tail, objectAPIFunc);
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
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_info3, "obj_funcGetInfo", api) != 1)
            goto RETURN_2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("func-tail", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info3);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func-tail(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info4 = MAKE_SYMBOL(getInfo_Obj, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) obj_func = MAKE_SYMBOL(isObjTrue, objectAPIFunc);
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
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_alc, "obj_funcGetArgCodeList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_vsl, "obj_funcGetVarList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_al, "obj_funcGetArgList", api) != 1)
            goto RETURN_2;
        if (addAPI(get_info4, "obj_funcGetInfo", api) != 1)
            goto RETURN_2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            goto RETURN_2;
        if (addAPI(obj_func, "obj_isObjFunc", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("func-obj", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
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
        printf("func-obj(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info5 = MAKE_SYMBOL(getInfo_Gc, objectAPIFunc);
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
        if (addAPI(get_info5, "obj_funcGetInfo", api) != 1)
            goto RETURN_2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            goto RETURN_2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("func-gc", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info5);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func-gc(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info9 = MAKE_SYMBOL(getInfo_Dynamic, objectAPIFunc);
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
        if (addAPI(get_info9, "obj_funcGetInfo", api) != 1)
            goto RETURN_2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            goto RETURN_2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("func-body-dynamic", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info9);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func-body-dynamic(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

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
        DLC_SYMBOL(objectAPIFunc) infix_func = MAKE_SYMBOL(isInfixTrue, objectAPIFunc);
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
        if (addAPI(infix_func, "obj_isInfixFunc", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("func-brackets", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
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
        printf("func-brackets(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info10 = MAKE_SYMBOL(getInfo_NotVar, objectAPIFunc);
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
        if (addAPI(get_info10, "obj_funcGetInfo", api) != 1)
            goto RETURN_2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            goto RETURN_2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("func-no-var", 3, 3, 3,
                                        (obj = makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info10);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func-no-var(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        af_Object *obj;
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl_Normal, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info11 = MAKE_SYMBOL(getInfo_Import, objectAPIFunc);
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
        if (addAPI(get_info11, "obj_funcGetInfo", api) != 1)
            goto RETURN_2;
        if (addAPI(free_mark, "obj_funcFreeMask", api) != 1)
            goto RETURN_2;
        if (addAPI(get_gl, "obj_getGcList", api) != 1)
            goto RETURN_2;

        makeVarToProtectVarSpace("func-import", 3, 3, 3,
                                        (obj =makeObject("func", true, api, true, nullptr, true, nullptr, env)), env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info11);
        FREE_SYMBOL(free_mark);
        FREE_SYMBOL(get_gl);
        FREE_SYMBOL(getSize_2);
        FREE_SYMBOL(initData_2);
        FREE_SYMBOL(freeData_2);
        printf("func-import(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

    {
        af_Object *obj = getGlobal(env);
        printf("global(%p)\n", obj);
        gc_delObjectReference(obj, env);
    }

    {  // 正常程序
        printf("\nTAG A: [Normal test]\n");
        af_Code *bt1 = makeElementCode("object", 0, 1, "Taga.aun");
        af_Code *bt2 = makeElementCode("data", 0, 0, nullptr);
        pushCode(&bt1, bt2);

        af_Code *bt3 = makeElementCode("func-normal", 0, 1, nullptr);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, nullptr, nullptr);
        pushCode(&bt1, bt5);

        af_Code *bt6 = makeElementCode("global", 0, 1, nullptr);
        pushCode(&bt5, bt6);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 宏函数
        printf("\nTAG L: [Macro test]\n");
        af_Code *bt1 = makeElementCode("object", 0, 1, "Tagl.aun");

        af_Code *bt3 = makeElementCode("macro", 0, 1, nullptr);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, nullptr, nullptr);
        pushCode(&bt1, bt5);

        af_Code *bt6 = makeElementCode("global", 0, 1, nullptr);
        pushCode(&bt5, bt6);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 测试顺序执行 '(xxx)
        printf("\nTAG E: [ex]\n");
        af_Code *bt3 = makeElementCode("data2", 0, 0, nullptr);
        af_Code *bt4 = makeElementCode("global", 0, 1, nullptr);

        pushCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(parentheses, bt3, '\'', 1, "Tage.aun", nullptr);

        runCodeFromMemory(bt5, 0, env);
        freeAllCode(bt5);
    }

    {  // 尾调用优化
        printf("\nTAG B: [Tail test]\n");
        af_Code *bt1 = makeElementCode("object", 0, 1, nullptr);

        af_Code *bt2 = makeElementCode("func-normal", 0, 1, nullptr);
        af_Code *bt3 = makeBlockCode(curly, bt2, 0, 1, nullptr, nullptr);
        pushCode(&bt1, bt3);

        af_Code *bt4 = makeBlockCode(parentheses, bt1, '\'', 1, "Tagb.aun", nullptr);

        runCodeFromMemory(bt4, 0, env);
        freeAllCode(bt4);
    }

    {  // 尾调用优化2
        printf("\nTAG C: [Tail test on literal]\n");
        af_Code *bt1 = makeElementCode("data", 0, 0, "Tagc.aun");
        af_Code *bt2 = makeBlockCode(parentheses, bt1, '\'', 1, "Tagc.aun", nullptr);

        runCodeFromMemory(bt2, 0, env);
        freeAllCode(bt2);
    }

    {  // 测试类前缀调用
        printf("\nTAG D: [parentheses-call]\n");
        af_Code *bt1 = makeElementCode("func-normal", 0, 1, "Tagd.aun");
        af_Code *bt2 = makeBlockCode(parentheses, nullptr, 0, 1, nullptr, nullptr);
        pushCode(&bt1, bt2);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }


    {  // 测试顺序执行 $[xxx]
        printf("\nTAG F: [ex-first]\n");
        af_Code *bt3 = makeElementCode("data2", 0, 0, nullptr);
        af_Code *bt4 = makeElementCode("global", 0, 1, nullptr);

        pushCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(brackets, bt3, '$', 1, "Tagf.aun", nullptr);

        runCodeFromMemory(bt5, 0, env);
        freeAllCode(bt5);
    }


    {  // 双层尾调用优化 （函数内调用函数）
        printf("\nTAG I: [Double tail test]\n");
        af_Code *bt1 = makeElementCode("func-tail", 0, 1, nullptr);
        af_Code *bt2 = makeBlockCode(curly, bt1, 0, 1, "Tagi.aun", nullptr);
        af_Code *bt3 = makeBlockCode(parentheses, bt2, '\'', 1, "Tagi.aun", nullptr);

        runCodeFromMemory(bt3, 0, env);
        freeAllCode(bt3);
    }

    {  // 对象函数的调用
        printf("\nTAG J: [object]\n");
        af_Code *bt1 = makeElementCode("func-obj", 0, 1, "Tagj.aun");

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 变量引用调用
        printf("\nTAG K: [not object]\n");
        af_Code *bt1 = makeElementCode("func-obj", '\'', 1, "Tagk.aun");

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 对象函数的调用 (尾调用优化)
        printf("\nTAG L: [pbject-tail]\n");
        af_Code *bt1 = makeElementCode("func-obj", 0, 1, "Tagl.aun");
        af_Code *bt2 = makeBlockCode(parentheses, bt1, '\'', 1, "Tagi.aun", nullptr);

        runCodeFromMemory(bt2, 0, env);
        freeAllCode(bt2);
    }


    {  // gc测试
        printf("\nTAG N: [gc-test]\n");

        af_Code *bt2 = makeElementCode("func-gc", 0, 1, nullptr);
        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, "Tagn.aun", nullptr);
        af_Code *bt3 = makeElementCode("global", 0, 1, nullptr);
        af_Code *bt4 = makeElementCode("global", 0, 1, nullptr);

        pushCode(&bt1, bt3);
        pushCode(&bt3, bt4);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // func_body_dynamic 测试
        printf("\nTAG O: [func body dynamic]\n");

        af_Code *bt2 = makeElementCode("func-body-dynamic", 0, 1, nullptr);
        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, "Tago.aun", nullptr);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 中缀调用测试
        printf("\nTAG P: [infix-call]\n");

        af_Code *bt2 = makeElementCode("func-brackets", 0, 1, nullptr);
        af_Code *bt1 = makeBlockCode(brackets, bt2, 0, 1, "Tagp.aun", nullptr);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // func_body_import 测试
        printf("\nTAG Q: [import]\n");

        af_Code *bt2 = makeElementCode("func-import", 0, 1, nullptr);
        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, "Tagq.aun", nullptr);
        af_Code *bt3 = makeElementCode("global", 0, 1, nullptr);

        pushCode(&bt1, bt3);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 导入式运行
        printf("\nTAG R: [import-Tagr]\n");
        af_Code *bt1 = makeElementCode("object", 0, 1, "Tagr.aun");
        af_Code *bt2 = makeElementCode("data", 0, 0, nullptr);
        pushCode(&bt1, bt2);

        af_Code *bt3 = makeElementCode("func-normal", 0, 1, nullptr);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, nullptr, nullptr);
        pushCode(&bt2, bt5);

        af_Code *bt6 = makeElementCode("global", 0, 1, nullptr);
        pushCode(&bt5, bt6);

        runCodeFromMemory(bt1, 1, env);
        freeAllCode(bt1);
    }

    {
        printf("\nTAG S: [string]\n");
        int exit_code = runCodeFromString("str\ndata\n{func-normal}\nglobal\n", "Tags-string.aun", 1, env);
        printf("exit code = %d\n", exit_code);
    }

    {
        DLC_SYMBOL(GuardianFunc) func = MAKE_SYMBOL(gd_func, GuardianFunc);
        DLC_SYMBOL(GuardianDestruct) des = MAKE_SYMBOL(gd_destruct, GuardianDestruct);
        struct GDData *data = nullptr;
        addGuardian("test", false, true, sizeof(struct GDData), func, des, (void **) &data, env);
        data->func = af_func;
        gc_addObjectReference(af_func, env);
        FREE_SYMBOL(func);
        FREE_SYMBOL(des);

        printf("\nTAG U: [guardian]\n");
        af_Code *bt1 = makeElementCode("func-normal", 0, 1, "TagU.aun");
        af_Code *bt2 = makeElementCode("global", 0, 1, "TagU.aun");
        af_Code *bt3 = makeElementCode("global", 0, 1, "TagU.aun");

        pushCode(&bt1, bt2);
        pushCode(&bt2, bt3);
        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);

        bool re = popGuardian("test", env);
        printf("popGuardian: %d\n\n", re);
    }

    {
        printf("\nTAG V: [Thread]\n");
        af_Code *bt1 = makeElementCode("object", 0, 1, "Tagv.aun");

        startRunThread(env, nullptr, bt1, false, true, true, true, true);
        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    // 错误用例

    {  // 中缀调用测试
        printf("\nTAG a: ERROR\n");

        af_Code *bt2 = makeElementCode("func-normal", 0, 1, nullptr);
        af_Code *bt1 = makeBlockCode(brackets, bt2, 0, 1, "Taga-error.aun", nullptr);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 测试错误 (无函数指定)
        printf("\nTAG b: ERROR\n");
        af_Code *bt1 = makeBlockCode(curly, nullptr, 0, 1, "Tagb-error.aun", nullptr);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 测试错误 (object2 Var not found)
        printf("\nTAG c: ERROR\n");
        af_Code *bt1 = makeElementCode("object2", 0, 1, "Tagc-error.aun");

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 中缀保护测试
        printf("\nTAG d: ERROR\n");

        af_Code *bt1 = makeElementCode("global", 0, 1, "Tagd-error.aun");
        af_Code *bt2 = makeElementCode("func-brackets", 0, 2, nullptr);

        pushCode(&bt1, bt2);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    {  // 错误回溯测试
        printf("\nTAG e: ERROR\n");

        af_Code *bt2 = makeElementCode("func-no-var", 0, 1, nullptr);
        af_Code *bt1 = makeBlockCode(curly, bt2, 0, 1, "Tage-error.aun", nullptr);

        af_Code *bt3 = makeElementCode("global", 0, 1, nullptr);
        pushCode(&bt1, bt3);

        runCodeFromMemory(bt1, 0, env);
        freeAllCode(bt1);
    }

    printf("\nfreeEnvironment:\n");
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