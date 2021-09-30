#include <assert.h>
#include "aFunCore.h"

#include "__run.h"
#include "__env.h"

/* 工具函数: 初始化类型 */
static bool iterCodeInit(af_Code *code, int mode, af_Environment *env);

/* 工具函数: Message类函数 */
static af_Message *getTopMsg(af_Environment *env);

/* 工具函数: 检查类型 */
static bool checkInMsgType(char *type, af_Environment *env);
static bool checkLiteral(af_Message **msg, af_Environment *env);
static int checkMacro(af_Message *msg, af_Environment *env);
static bool checkRunGC(af_Environment *env);
static int checkMsg(af_Message *msg, af_Environment *env);
bool checkNormalEnd(af_Message *msg, af_Environment *env);
static bool checkGetArgEnd(af_Message *msg, af_Environment *env);

/* Code 执行函数 */
static bool codeElement(af_Code *code, af_Environment *env);
static bool codeBlock(af_Code *code, af_Environment *env);

/*
 * 函数名: checkInMsgType
 * 目标: 检查type是否在对应的msg_type中 (检查该activity是否可以处理该信息)
 */
static bool checkInMsgType(char *type, af_Environment *env) {
    if (env->activity->body_next == NULL || env->activity->body_next->msg_type == NULL)
        return false;
    for (char *msg_type_node = *env->activity->body_next->msg_type; msg_type_node != NULL; msg_type_node++) {
        if (EQ_STR(type, msg_type_node))
            return true;
    }
    return false;
}

/*
 * 函数名: checkLiteral
 * 目标: 检查是否字面量调用, 若是则返回true并调用API, 否则返回false不做修改
 */
static bool checkLiteral(af_Message **msg, af_Environment *env) {
    if (!env->activity->is_literal)  // 非字面量
        return true;
    if ((*msg) == NULL || !EQ_STR((*msg)->type, "NORMAL"))  // (*msg)非正常值
        return false;

    af_Object *obj = *(af_Object **)((*msg)->msg);
    obj_literalSetting *func = findAPI("obj_literalSetting", obj->data->api);
    if (func == NULL) {
        gc_delReference(obj);
        freeMessage(*msg);
        *msg = makeERRORMessage(TYPE_ERROR, API_NOT_FOUND_INFO(obj_literalSetting), env);
        return false;
    }

    for (af_LiteralDataList *ld = env->activity->ld; ld != NULL; ld = ld->next)
        func(obj->data->id, obj, obj->data->data, ld->literal_data, env);

    freeAllLiteralData(env->activity->ld);
    env->activity->ld = NULL;
    env->activity->is_literal = false;
    writeInfoLog(aFunCoreLogger, "Literal %p", obj);
    return true;
}

/*
 * 函数名: checkMacro
 * 目标: 检查是否宏函数调用, 若是则返回true并修改activity隐式调用(activity继续执行时则会执行该调用), 否则返回false不做修改
 * 返回值:
 * -1 非宏函数
 *  0 错误
 *  1 宏函数
 */
static int checkMacro(af_Message *msg, af_Environment *env) {
    if (env->activity->fi == NULL || !env->activity->fi->is_macro)  // 非宏函数
        return -1;
    if (!EQ_STR(msg->type, "NORMAL"))  // msg非正常值
        return -1;

    af_Object *obj = *(af_Object **)(msg->msg);
    bool re = pushMacroFuncActivity(obj, env);
    gc_delReference(obj);
    freeMessage(msg);
    if (re)
        return 1;
    return 0;
}

/*
 * 函数名: checkRunGC
 * 目标: 检查是否该运行gc, 若是则返回true并运行gc, 否则返回false
 */
static bool checkRunGC(af_Environment *env) {
    if (env->core->gc_runtime->num == grt_always ||
        env->core->gc_runtime->num == grt_count && env->core->gc_count->num >= env->core->gc_max->num) {
        env->core->gc_count->num = 0;  // 清零
        gc_RunGC(env);
        return true;
    }
    return false;
}

/*
 * 函数名: iterCodeInit
 * 目标: 初始化activity和environment (若environment中未存在activity则通过code新增一个TopActivity, 否则沿用原activity)
 *
 * mode 标识运行模式
 * 0. 在top运行
 * 1. 在import运行
 * 2. 直接运行
 * 3. gc模式
 */
static bool iterCodeInit(af_Code *code, int mode, af_Environment *env) {
    if (env == NULL || env->core == NULL || env->activity == NULL || env->core->status == core_exit || env->in_run)
        return false;
    if (env->core->status == core_stop)
        env->core->status = core_normal;

    switch (mode) {
        case 0:
            if (env->activity->type != act_top || !codeSemanticCheck(code))
                return false;
            setActivityBtTop(NULL, env->activity);  // 直接就在NORMAL期, bt_top不被设定
            setActivityBtStart(code, env->activity);
            break;
        case 1: {
            if (env->activity->type != act_top)
                return false;
            env->activity->file = strCopy("top.aun.sys");

            char *name = getFileName(code->path);
            pushImportActivity(code, NULL, name, env);
            writeInfoLog(aFunCoreLogger, "Top-Import name = %s", name);
            free(name);
            break;
        }
        case 2:
            if (env->activity->prev == NULL || env->activity->prev->type != act_top)
                return false;
            env->activity->file = strCopy("top-gc.aun.sys");

            if (env->activity->type == act_gc || !codeSemanticCheck(env->activity->bt_start) || env->activity->bt_next == NULL || code != NULL)
                return false;
            break;
        case 3:
            if (env->activity->type != act_gc || code != NULL)
                return false;
            break;
        default:
            return false;
    }

    env->in_run = true;
    return true;
}

/*
 * 函数名: codeElement
 * 目标: 执行变量访问或字面量生成 (设置bt_next)
 * (1) 执行字面量生成代码 (设置bt_next)
 *     返回-false 表示执行错误 (msg_down中写入消息)
 *     返回-true  表示执行成功 (msg_down中无消息写入, 函数仅设置activity无实质性代码执行)
 * (2) 执行变量访问代码:
 *     返回-false 表示执行失败, 或执行成功得到一个变量值      (msg_down中写入消息)
 *     返回-true  表示执行成功, 得到一个对象函数, 并且隐式调用 (msg_down中无消息写入, 函数仅设置activity无实质性代码执行)
 */
static bool codeElement(af_Code *code, af_Environment *env) {
    bool in_protect;
    char *func;
    af_Var *var;

    if (checkLiteralCode(code->element.data, &func, &in_protect, env)) {
        /* 字面量执行 */
        if (in_protect)
            var = findVarFromVarSpace(func, env->activity->belong, env->core->protect);
        else
            var = findVarFromVarList(func, env->activity->belong, env->activity->vsl);

        if (var == NULL) {
            pushMessageDown(makeERRORMessageFormat(LITERAL_ERROR, env, "Literal not found: %s: %s.", code->element.data, func), env);
            return false;
        }

        return pushLiteralActivity(code, code->element.data, var->vn->obj, env);
    }

    /* 变量执行 */
    var = findVarFromVarList(code->element.data, env->activity->belong, env->activity->vsl);

    if (var == NULL) {
        pushMessageDown(makeERRORMessageFormat(VARIABLE_ERROR, env, "Variable not found: %s.", code->element.data), env);
        return false;
    }

    af_Object *obj = var->vn->obj;
    obj_isObjFunc *is_obj;
    obj_isInfixFunc *is_infix;

    if (code->prefix != getPrefix(E_QUOTE, env)) {
        if ((is_obj = findAPI("obj_isObjFunc", obj->data->api)) != NULL && is_obj(obj->data->id, obj))
            return pushVariableActivity(code, var->vn->obj, env);  // 对象函数
        else if (env->activity->status != act_func_get && // 在act_func_get 模式下不检查是否为is_infix函数 因为本来就要将其作为函数调用
                 (is_infix = findAPI("obj_isInfixFunc", obj->data->api)) != NULL && is_infix(obj->data->id, obj)) {
            pushMessageDown(makeERRORMessageFormat(INFIX_PROTECT, env,
                                                   "Infix protect variable: %s.", code->element.data), env);
            return false;
        }
    }

    pushMessageDown(makeNORMALMessage(obj), env);
    setActivityBtNext(env->activity->bt_next->next, env->activity);
    writeInfoLog(aFunCoreLogger, "Get Variable %s : %p", code->element.data, obj);
    return false;
}

/*
 * 函数名: codeBlock
 * 目标: 执行括号语法 (顺序执行, 函数调用)
 * 返回-false 表示执行错误 (msg_down中写入消息)
 * 返回-true  表示执行成功 (msg_down中无消息写入, 函数仅设置activity无实质性代码执行)
 */
static bool codeBlock(af_Code *code, af_Environment *env) {
    if (code->prefix == getPrefix(B_EXEC, env) && code->block.type == parentheses)  // 顺序执行, 返回尾项
        return pushExecutionActivity(code, false, env);
    else if (code->prefix == getPrefix(B_EXEC_FIRST, env) && code->block.type == brackets)  // 顺序执行, 返回首项
        return pushExecutionActivity(code, true, env);
    else
        return pushFuncActivity(env->activity->bt_next, env);
}


/*
 * 函数名: getTopMsg
 * 目标: 获取msg_down顶层信息, 若没有则产生错误
 */
static af_Message *getTopMsg(af_Environment *env) {
    if (env->activity->msg_down == NULL)  // 若未获得 msg
        return makeERRORMessage(RUN_ERROR, NOT_MSG_INFO, env);
    else
        return getFirstMessage(env);
}

/*
 * 函数名: checkMsg
 * 目标: 检查msg是否为NORMAL, 并且检查该信号是否可被处理
 * 返回- (1) 正常
 * 返回- (0) 不可处理的信号
 * 返回-(-1) 非正常但通过msg_type判断为可被处理的信号
 */
static int checkMsg(af_Message *msg, af_Environment *env) {
    if (EQ_STR(msg->type, "NORMAL"))
        return 1;  // 正常

    pushMessageDown(msg, env);  // msg不弹出
    if (env->activity->status != act_func_normal || !checkInMsgType(msg->type, env)) {  // 非normal模式, 或normal模式下msg_type不匹配该msg
        env->activity->return_first = false;
        env->activity->return_obj = NULL;
        return 0;
    }

    return -1;  // 遇到非normal, 但继续运行
}

/*
 * 函数名: checkNormalEnd
 * 目标: 检查act_normal是否运行到结尾 (若运行到结尾则返回true, 否则返回false)
 */
bool checkNormalEnd(af_Message *msg, af_Environment *env) {
    if (env->activity->bt_next == NULL) {
        if (env->activity->type == act_top || env->activity->type == act_top_import) {
            pushMessageDown(msg, env);
            return true;
        } else if (setFuncActivityToNormal(env) == 0) {  // 已经没有下一步了
            if (msg == NULL) {  // msg 得不到处理
                pushMessageDown(makeERRORMessage(RUN_ERROR, NOT_NORMAL_MSG_INFO, env), env);
                return true;
            }

            switch (checkMacro(msg, env)) {
                case 0:
                    return true;  // 错误
                case 1:
                    return false;  // 宏函数
                case -1:
                default:
                    break;  // 非宏函数
            }

            checkLiteral(&msg, env);  // 检查是否字面量
            pushMessageDown(msg, env);
            return true;
        } else if (msg != NULL) {
            gc_delReference(*(af_Object **) (msg->msg));  // msg->msg是一个指针, 这个指针的内容是一个af_Object *
            freeMessage(msg);
        }
    } else if (msg != NULL) {
        if (env->activity->bt_next->type == code_block && env->activity->bt_next->block.type == parentheses &&
            env->activity->bt_next->prefix != getPrefix(B_EXEC, env)) {
            env->activity->parentheses_call = *(af_Object **) (msg->msg);  // 类前缀调用
        }
        gc_delReference(*(af_Object **)(msg->msg));  // msg->msg是一个指针, 这个指针的内容是一个af_Object *
        freeMessage(msg);
    }
    return false;
}

/*
 * 函数名: checkGetArgEnd
 * 目标: 检查act_arg是否运行到结尾 (若运行到结尾则返回true, 否则返回false)
 */
static bool checkGetArgEnd(af_Message *msg, af_Environment *env) {
    env->activity->acl_done->result = *(af_Object **)(msg->msg);
    freeMessage(msg);
    if (env->activity->acl_done->next == NULL) { // 参数设定结束
        setArgCodeListToActivity(NULL, env);
        return true;
    }
    env->activity->acl_done = env->activity->acl_done->next;
    setArgCodeListToActivity(env->activity->acl_done, env);
    return false;
}

static bool checkStop(af_Environment *env) {
    if (env->core->status == core_stop || env->core->status == core_exit) {
        while (env->activity->type != act_top || env->activity->prev != NULL)
            popActivity(false, NULL, env);  // is_normal=false, 非正常退出, 释放mark
        popActivity(false, NULL, env);  // 再释放 act_top
        return true;
    }
    return false;
}

/*
 * 函数名: iterCode
 * 目标: 运行代码 (代码可通过code参数传入, 或通过env->activity传入)
 * 注意: 曾为缩短改函数而将该函数的内容进行大量的封装
 *      但实际上, 这个函数各个部分的关联性是很强的
 *      因此, 取消了封装, 反而提高了代码的可读性
 *
 *      因为该函数的大部分内容运行在循环中, 因此使用continue表示不在运行后面的代码
 */
bool iterCode(af_Code *code, int mode, af_Environment *env){
    if (!iterCodeInit(code, mode, env))
        return false;

    /*
     * 问题: 如何确保循环跳出之前, top-Activity已经被pop。(即执行释放)
     * 为什么会有这个问题: top-Activity只有在bt_next=NULL时被pop, 而循环也是在bt_next=NULL时可能被退出
     *                  如此以来就可能导致在pop之前循环就退出了
     * 实际上并不会发生。
     * bt_next设定后，会出现两种情况: 一是马上检查bt_next, 而是设定了pass
     * 设定了pass是意味着压入新的activity。当新的activity被返回时, 必定设置了process_msg_first
     * 而process_msg_first时, 也会检查bt_next
     *
     * 【run-code设置了bt_next】 -> 检查bt_next并可能做pop处理 -> while循环检查bt_next  [例如变量访问语句]
     * 【run-code设置了bt_next】 -> 压入了新的activity -> while循环检查 和一系列运行
     *  -> 新activity返回, 设定process_msg_first -> while循环检查 (因为process_msg_first, 所以不会跳出循环)
     *  -> process_msg_first会处理msg, 检查bt_next.
     *
     * popActivity会是一定会设置process_msg_first, 除了gc机制。
     * 而gc机制前后, bt_next不会改变，这意味着如果gc之后while循环就会被跳出, 那么gc之前while循环早就跳出了
     */

    /* 必须位于act_top, 且无next, 并且无msg处理才退出执行 */
    while (env->activity->type != act_top || env->activity->bt_next != NULL || env->activity->process_msg_first != 0) {
        /* 检查是否需要退出执行 */
        if (checkStop(env))
            goto RETURN_FALSE;

        /* 检查gc机制 */
        checkRunGC(env);

        if (env->activity->type == act_gc) {
            if (env->activity->dl_next == NULL)
                popActivity(true, NULL, env);  // 结束运行
            else
                pushDestructActivity(env->activity->dl_next, env);
            continue;
        }

        /* 切换执行的var_list */
        if (env->activity->type == act_func && env->activity->status == act_func_arg &&
            env->activity->run_in_func && env->activity->func_var_list != NULL)
            env->activity->vsl = env->activity->func_var_list;
        else
            env->activity->vsl = env->activity->var_list;

        /* 无代码运行 */
        if (env->activity->bt_next == NULL && env->activity->process_msg_first == 0) {  // 无代码运行, 并且非msg_first
            switch (env->activity->status) {
                case act_func_get:
                case act_func_normal:
                    popActivity(false, makeERRORMessage(RUN_ERROR, NOT_CODE_INFO, env), env);
                    break;
                case act_func_arg:  // 无参数设定
                    if (!setFuncActivityAddVar(env))
                        popActivity(false, NULL, env);
                    break;
                default:
                    break;
            }
            continue;  // 后面的代码不再运行
        }

        /* 有代码运行 */
        bool pass_msg = false;  // 表示不处理msg
        if (env->activity->process_msg_first == 0) {  /* 运行实际代码 */
            switch (env->activity->bt_next->type) {
                case code_element:
                    if (codeElement(env->activity->bt_next, env))
                        pass_msg = true;
                    break;
                case code_block:
                    if (codeBlock(env->activity->bt_next, env))
                        pass_msg = true;  // 若运行成功则跳转到下一次运行, 该步骤仅为设置Activity
                    break;
                default:
                    break;
            }
        } else
            env->activity->process_msg_first--;

        if (pass_msg)
            continue;  // 后面的代码不再运行

        /* 处理msg */
        af_Message *msg = getTopMsg(env);
        switch (checkMsg(msg, env)) {
            case 0:  // 不可处理的信号
                popActivity(false, NULL, env);  // 跳出当前activity
                continue;  // 下面的代码不再执行
            case 1:  // 正常信号
                if (env->activity->return_first && env->activity->return_obj == NULL)  // 设置return_first
                    env->activity->return_obj = *(af_Object **)msg->msg;
                break;
            case -1:  // 非正常但可处理 [已经放回]
            default:
                assert(env->activity->status == act_func_normal);
                break;
        }

        switch (env->activity->status) {
            case act_func_normal:  // 需要考虑 msg == NULL
                if (checkNormalEnd(msg, env))
                    popActivity(true, NULL, env);  // 正常退出
                break;
            case act_func_get: {
                af_Object *func = *(af_Object **) (msg->msg);  // func仍保留了msg的gc计数
                gc_delReference(func);  // 释放计数
                freeMessage(msg);
                if (!setFuncActivityToArg(func, env))
                    popActivity(false, NULL, env);
                break;
            }
            case act_func_arg: {
                if (checkGetArgEnd(msg, env)) {  // 参数设定完成
                    if (!setFuncActivityAddVar(env))
                        popActivity(false, NULL, env);
                }
                break;
            }
            default:
                break;
        }
    }

    env->in_run = false;
    return true;

RETURN_FALSE:
    env->in_run = false;
    return false;
}

/*
 * 函数名: iterDestruct
 * 目标: 对所有ObjectData执行析构函数
 * 会循环不断检测是否有新增ObjectData并且需要析构
 * deep - 表示最大迭代深度 (设置为0表示不限制)
 */
bool iterDestruct(int deep, af_Environment *env) {
    for (int count = 0; deep == 0 || deep > count; count++) {
        gc_DestructList *dl = NULL;
        pgc_DestructList pdl = &dl;

        pdl = checkAllDestruct(env, pdl);
        if (dl == NULL)
            return true;
        pushGCActivity(dl, pdl, env);
        if (!iterCode(NULL, 3, env))
            return false;
    }
    return false;
}
