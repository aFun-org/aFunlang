#include "__aFun_tool.h"
#include "__rt_tool.h"

/* 数组为只读的内容 */
static const ToolFunc global_tool_list[] = {
        {.name="base", .func=aFunTool_base},
        {.name=NULL, .func=NULL},
};

/*
 * 返回 (0)   执行正常
 * 返回 (1)  库不存在
 *
 * 必须传入: code
 */
int aFunTool(char *name, af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    if (code == NULL || env == NULL || vs == NULL)
        return 1;
    *code = NULL;
    for (const ToolFunc *tf = global_tool_list; global_tool_list->name != NULL; tf++) {
        if (EQ_STR(tf->name, name)) {
            int exit_code = tf->func(code, visitor, vs, env);
            if (exit_code == 1)  // 将 exit_code 映射到0
                return 0;
            return exit_code;
        }
    }
    return 1;
}

/*
 * 函数名: makeAPIFromList
 * 目标: 根据APIFunc生成api表并写入数据
 */
af_ObjectAPI *makeAPIFromList(const APIFunc api_list[]) {
    af_ObjectAPI *api = makeObjectAPI();

    for (const APIFunc *af = api_list; af->name != NULL; af++) {
        DLC_SYMBOL(objectAPIFunc) func = MAKE_SYMBOL(af->func, objectAPIFunc);
        addAPI(func, af->name, api);
        FREE_SYMBOL(func);
    }

    return api;
}

/*
 * 函数名: makeObjectFromList
 * 目标: 根据ObjectDefine生成Object, 并保存到对应位置和变量空间中
 */
void makeObjectFromList(const ObjectDefine obj_def[], af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    for (const ObjectDefine *od = obj_def; od->id != NULL; od++) {
        af_ObjectAPI *api = od->api;
        if (api == NULL)
            api = makeAPIFromList(od->api_list);

        af_Object *obj = makeObject(od->id, od->free_api, api, od->allow_inherit, od->belong, od->inherit, env);
        if (od->save != NULL)
            *(od->save) = obj;
        if (vs != NULL && od->var_name != NULL) {
            if (!makeVarToVarSpace(od->var_name, od->p_self, od->p_posterity, od->p_external, obj, vs, visitor, env))
                setVarToVarSpace(od->var_name, obj, visitor, vs);  // 若失败则尝试直接设定变量
        }
    }
}

/*
 * 函数名: makeLiteralRegexFromList
 * 目标: 根据LiteralFunc压入新的字面量处理器
 */
void makeLiteralRegexFromList(const LiteralFunc literal_list[], af_Environment *env) {
    for (const LiteralFunc *lt = literal_list; lt->pattern != NULL; lt++)
        pushLiteralRegex(lt->pattern, lt->func, lt->in_protect, env);
}

/*
 * 函数名: makeTopMsgProcessFromList
 * 目标: 根据TopMsgFunc压入新的字面量处理器
 */
void makeTopMsgProcessFromList(const TopMsgFunc top_msg_list[], af_Environment *env) {
    for (const TopMsgFunc *tml = top_msg_list; tml->type != NULL; tml++) {
        DLC_SYMBOL(TopMsgProcessFunc) func = MAKE_SYMBOL(tml->func, TopMsgProcessFunc);
        addTopMsgProcess(tml->type, func, env);
        FREE_SYMBOL(func);
    }
}