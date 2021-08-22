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

int main() {
    aFunInit();
    printf("Hello World\n");

    af_Environment *env = makeEnvironment();
    addVarToProtectVarSpace(makeVar("global", 3, 3,
                                    makeObject("global", true, makeObjectAPI(), true, NULL, NULL, env)),
                            env);
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

        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("object", 0, 1, NULL);

        connectCode(&bt1, bt2);
        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        af_Code *bt6 = makeVariableCode("global", 0, 1, NULL);
        connectCode(&bt5, bt6);

        iterCode(bt1, env);
        freeAllCode(bt1);
    }

    {  // 尾调递归优化
        af_Code *bt1 = makeLiteralCode("data", "func", false, ',', 0, "Unknow");
        af_Code *bt2 = makeVariableCode("object", 0, 1, NULL);

        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("object", 0, 1, NULL);

        connectCode(&bt1, bt2);
        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        iterCode(bt1, env);
        freeAllCode(bt1);
    }

    {  // 测试类前缀调用
        af_Code *bt1 = makeLiteralCode("data", "func", false, ',', 0, "Unknow");
        af_Code *bt2 = makeVariableCode("object", 0, 1, NULL);

        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("object", 0, 1, NULL);

        connectCode(&bt1, bt2);
        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(parentheses, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        iterCode(bt1, env);
        freeAllCode(bt1);
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
    }

    {  // 测试顺序执行 '(xxx) 【尾调递归优化】
        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(parentheses, bt3, '\'', 1, NULL, NULL);

        iterCode(bt5, env);
        freeAllCode(bt5);
    }

    {  // 测试顺序执行 ,[xxx] 【尾调递归优化】
        af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);

        connectCode(&bt3, bt4);

        af_Code *bt5 = makeBlockCode(brackets, bt3, ',', 1, NULL, NULL);

        iterCode(bt5, env);
        freeAllCode(bt5);
    }

    freeEnvironment(env);
    return 0;
}