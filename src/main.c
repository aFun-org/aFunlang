#include <stdio.h>
#include "aFun.h"

void mp_ERROR_STR(af_Message *msg, af_Environment *env) {
    char **pinfo = getMessageData(msg);
    printf("ERROR\n");
    if (pinfo == NULL || *pinfo == NULL)
        return;
    fprintf(stderr, "ERROR-STR INFO : %s\n", *pinfo);
    free(*pinfo);
}

size_t getSize(void) {
    return sizeof(int *);
}

void initData(int **data) {
    *data = calloc(sizeof(int), 1);
    **data = 100;
}

void freeData(int **data) {
    printf("freeData(): **data = %d\n", **data);
    free(*data);
}

bool getAcl(ArgCodeList **acl, af_Object *obj, af_Code *code, int **mark, af_Environment *env) {
    *acl = makeArgCodeList(makeVariableCode("object", NUL, 0, "Unknown"), 0, true, false);
    *mark = calloc(sizeof(int), 1);
    **mark = 100;
    return true;
}

bool getVsl(af_VarSpaceListNode **vsl, af_Object *obj, void *mark, af_Environment *env) {
    *vsl = makeVarSpaceList(makeVarSpace());
    return true;
}

bool getAl(ArgList **al, af_Object *obj, ArgCodeList *acl, void *mark, af_Environment *env) {
    *al = makeArgList("test", getArgCodeListResult(acl));
    return true;
}

void testFunc(int *mark, af_Environment *env) {  // 测试用函数
    printf("testFunc(): I am testFunc\n");

    af_Object *obj = makeObject("Literal", true, makeObjectAPI(), true, NULL, NULL, env);
    af_Message *msg = makeMessage("NORMAL", sizeof(af_Object *));
    *((af_Object **)(getMessageData(msg))) = obj;  // TODO-szh 没有gc
    pushMessageDown(msg, env);
}

bool getInfo(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env) {
    *fi = makeFuncInfo(normal_scope, not_embedded, false, false);  // 获取FuncInfo [桩]
    makeCodeFuncBodyToFuncInfo(makeVariableCode("test", NUL, 0, "Unknow"), true, NULL, *fi);

    DLC_SYMBOL(callFuncBody) func = MAKE_SYMBOL(testFunc, callFuncBody);
    makeCFuncBodyToFuncInfo(func, NULL, *fi);
    FREE_SYMBOL(func);
    return true;
}

void freeMark(int *mark) {
    printf("freeMark(): mark = %d\n", *mark);
    free(mark);
}


int main() {
    aFunInit();
    printf("Hello World\n");

    af_Environment *env = makeEnvironment();
    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) getSize_ = MAKE_SYMBOL(getSize, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) initData_ = MAKE_SYMBOL(initData, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) freeData_ = MAKE_SYMBOL(freeData, objectAPIFunc);
        if (addAPI(getSize_, "obj_getDataSize", api) != 1)
            return 2;
        if (addAPI(initData_, "obj_initData", api) != 1)
            return 2;
        if (addAPI(freeData_, "obj_freeData", api) != 1)
            return 2;

        addVarToProtectVarSpace(makeVar("global", 3, 3,
                                        makeObject("global", true, api, true, NULL, NULL, env)),
                                env);
        FREE_SYMBOL(getSize_);
        FREE_SYMBOL(initData_);
        FREE_SYMBOL(freeData_);
    }

    {
        af_ObjectAPI *api = makeObjectAPI();
        DLC_SYMBOL(objectAPIFunc) get_alc = MAKE_SYMBOL(getAcl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_vsl = MAKE_SYMBOL(getVsl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_al = MAKE_SYMBOL(getAl, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) get_info = MAKE_SYMBOL(getInfo, objectAPIFunc);
        DLC_SYMBOL(objectAPIFunc) free_mark = MAKE_SYMBOL(freeMark, objectAPIFunc);
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

        addVarToProtectVarSpace(makeVar("func", 3, 3,
                                        makeObject("func", true, api, true, NULL, NULL, env)),
                                env);
        FREE_SYMBOL(get_alc);
        FREE_SYMBOL(get_vsl);
        FREE_SYMBOL(get_al);
        FREE_SYMBOL(get_info);
        FREE_SYMBOL(free_mark);
    }

    addVarToProtectVarSpace(makeVar("object", 3, 3,
                                    makeObject("object", true, makeObjectAPI(), true, NULL, NULL, env)),
                            env);

    {
        DLC_SYMBOL(TopMsgProcessFunc) func = MAKE_SYMBOL(mp_ERROR_STR, TopMsgProcessFunc);
        addTopMsgProcess("ERROR-STR", func, env);
        FREE_SYMBOL(func);
    }

    if (!enableEnvironment(env)) {
        fprintf(stderr, "Enable Error.\n");
        exit(EXIT_FAILURE);
    }

    {  // 正常程序
        af_Code *bt1 = makeLiteralCode("data", "func", false, ',', 0, "Unknow");
        af_Code *bt2 = makeVariableCode("object", 0, 1, NULL);
        connectCode(&bt1, bt2);

        af_Code *bt3 = makeVariableCode("func", 0, 1, NULL);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        af_Code *bt6 = makeVariableCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 尾调递归优化
        af_Code *bt1 = makeLiteralCode("data", "func", false, ',', 0, "Unknow");
        af_Code *bt2 = makeVariableCode("object", 0, 1, NULL);
        connectCode(&bt1, bt2);

        af_Code *bt3 = makeVariableCode("func", 0, 1, NULL);
        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 测试类前缀调用
        af_Code *bt1 = makeLiteralCode("data", "func", false, ',', 0, "Unknow");
        af_Code *bt2 = makeVariableCode("func", 0, 1, NULL);
        connectCode(&bt1, bt2);

        af_Code *bt3 = makeVariableCode("global", 0, 1, NULL);
        af_Code *bt5 = makeBlockCode(parentheses, bt3, 0, 1, NULL, NULL);  // TODO-szh 参数个数允许为0
        connectCode(&bt2, bt5);

        af_Code *bt6 = makeVariableCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 测试错误 (object2 Var not found)
        af_Code *bt1 = makeLiteralCode("data", "func", false, ',', 0, "Unknow");
        af_Code *bt2 = makeVariableCode("object2", 0, 1, NULL);

        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);
        af_Code *bt6 = makeVariableCode("object", 0, 1, NULL);

        connectCode(&bt1, bt2);
        connectCode(&bt3, bt4);
        connectCode(&bt4, bt6);

        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        iterCode(bt1, env);
        freeAllCode(bt1);
        printf("\n");
    }

    {  // 测试顺序执行 '(xxx)
        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(parentheses, bt3, '\'', 1, NULL, NULL);

        af_Code *bt6 = makeVariableCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt5, env);
        freeAllCode(bt5);
        printf("\n");
    }

    {  // 测试顺序执行 ,[xxx]
        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(brackets, bt3, ',', 1, NULL, NULL);

        af_Code *bt6 = makeVariableCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt5, env);
        freeAllCode(bt5);
        printf("\n");
    }

    {  // 测试顺序执行 '(xxx) 【尾调递归优化】
        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(parentheses, bt3, '\'', 1, NULL, NULL);

        iterCode(bt5, env);
        freeAllCode(bt5);
        printf("\n");
    }

    {  // 测试顺序执行 ,[xxx] 【尾调递归优化】
        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(brackets, bt3, ',', 1, NULL, NULL);

        iterCode(bt5, env);
        freeAllCode(bt5);
        printf("\n");
    }

    freeEnvironment(env);
    return 0;
}