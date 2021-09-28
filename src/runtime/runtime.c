#include "__runtime.h"
#include "__global_obj.h"
#include "runtime_tool.h"

/* 数组为只读的内容 */
static const ToolFunc global_tool_list[] = {
        {.name="base", .func=aFunTool_base},
        {.name=NULL, .func=NULL},
};

/*
 * 函数名: runtimeTool
 * 目标: 调用指定内置包(tool)
 * 返回 (0)   执行正常
 * 返回 (1)  库不存在
 */
int runtimeTool(char *name, af_Code **code, af_Object *visitor, af_VarSpace *vs, af_Environment *env) {
    if (name == NULL || code == NULL || env == NULL || vs == NULL)
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
 * 函数名: runtimeToolImport
 * 目标: 生成vs, 调用指定内置包(tool)
 */
int runtimeToolImport(char *name, af_Object **obj, af_Code **code, af_Environment *env) {
    if (name == NULL || code == NULL || env == NULL || obj == NULL)
        return 1;
    *obj = makeGlobalObject(env);
    return runtimeTool(name, code, *obj, (*obj)->data->var_space, env);
}

/*
 * 函数名: makeAPIFromList
 * 目标: 根据APIFunc生成api表并写入数据
 */
af_ObjectAPI *makeAPIFromList(const APIFunc api_list[]) {
    af_ObjectAPI *api = makeObjectAPI();

    for (const APIFunc *af = api_list; af->name != NULL; af++) {
        if (af->func != NULL) {
            DLC_SYMBOL(objectAPIFunc) func = MAKE_SYMBOL_FROM_HANDLE(af->func, af->dlc, objectAPIFunc);
            addAPI(func, af->name, api);
            FREE_SYMBOL(func);
            continue;
        } else if (af->func_ == NULL)
            continue;  // 遇到错误

        addAPI(af->func_, af->name, api);
        if (af->free_func_)
            FREE_SYMBOL(af->func_);
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
        if (tml->func != NULL) {
            DLC_SYMBOL(TopMsgProcessFunc) func = MAKE_SYMBOL(tml->func, TopMsgProcessFunc);
            addTopMsgProcess(tml->type, func, env);
            FREE_SYMBOL(func);
            continue;
        } else if (tml->func_ == NULL)
            continue;  // 遇到错误

        addTopMsgProcess(tml->type, tml->func_, env);
        if (tml->free_func_)
            FREE_SYMBOL(tml->func_);
    }
}

/*
 * 函数名: makeInheritFromListReverse
 * 目标: 以InheritDefine的顺序反向压入Inherit
 * 注意: pushInherit是反向压入, 因此InheritDefine得正向读取, 最后Inherit反向压入
 */
static af_Inherit *makeInheritFromListReverse(const InheritDefine inherit_list[]) {
    af_Inherit *inherit = NULL;
    af_Inherit **pinherit = &inherit;
    for (const InheritDefine *ind = inherit_list; ind->obj != NULL; ind++) {
        af_Inherit *ih = makeInherit(ind->obj);
        pinherit = pushInherit(pinherit, ih);
    }

    return inherit;
}

/*
 * 函数名: makeInheritFromListForward
 * 目标: 以InheritDefine的顺序压入Inherit
 * 注意: pushInherit是反向压入, 因此InheritDefine也得反向读取, 最后Inherit正向压入
 */
static af_Inherit *makeInheritFromListForward(const InheritDefine inherit_list[]) {
    af_Inherit *inherit = NULL;
    af_Inherit **pinherit = &inherit;
    const InheritDefine *ind = inherit_list;

    /* 找到最后一个元素 */
    while (ind->obj != NULL)
        ind++;
    ind--;  // 最后一个元素的前一个元素为最后一个有效元素

    for (NULL; ind != inherit_list; ind++) {
        af_Inherit *ih = makeInherit(ind->obj);
        pinherit = pushInherit(pinherit, ih);
    }

    return inherit;
}

/*
 * 函数名: makeInheritFromList
 * 目标: 根据InheritDefine生成新的Inherit
 */
af_Inherit *makeInheritFromList(const InheritDefine inherit_list[], bool is_reverse) {
    if (is_reverse)
        return makeInheritFromListReverse(inherit_list);
    else
        return makeInheritFromListForward(inherit_list);
}
