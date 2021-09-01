#include "aFun.h"

#include "run.h"
#include "__env.h"

/* 工具函数: 初始化类型 */
static bool iterCodeInit(af_Code *code, af_Environment *env);

/* 工具函数: 运行函数 */
static void setRunVarSpaceList(af_Environment *env);
static bool runCodeBase(af_Environment *env);
static bool runCode(af_Message **msg, bool *run_code, af_Environment *env);
static void processMsg(af_Message *msg, bool run_code, af_Environment *env);

/* 工具函数: Message类函数 */
static af_Message *getTopMsg(af_Environment *env);

/* 工具函数: 检查类型 */
static bool checkInMsgType(char *type, af_Environment *env);
static bool checkLiteral(af_Message **msg, af_Environment *env);
static bool checkMacro(af_Message *msg, af_Environment *env);
static bool checkRunGC(af_Environment *env);
static int checkMsg(af_Message *msg, af_Environment *env);
bool checkNormalEnd(af_Message *msg, af_Environment *env);
static bool checkGetArgEnd(af_Message *msg, af_Environment *env);

/* Code 执行函数 */
static bool codeElement(af_Code *code, af_Environment *env);
static bool codeLiteral(af_Code *code, af_Environment *env);
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
        func(ld->literal_data, obj->data->data, obj, env);

    freeAllLiteralData(env->activity->ld);
    env->activity->ld = NULL;
    env->activity->is_literal = false;
    printf("Literal %p\n", obj);
    return true;
}

/*
 * 函数名: checkMacro
 * 目标: 检查是否宏函数调用, 若是则返回true并修改activity隐式调用(activity继续执行时则会执行该调用), 否则返回false不做修改
 */
static bool checkMacro(af_Message *msg, af_Environment *env) {
    if (env->activity->fi == NULL || !env->activity->fi->is_macro)  // 非宏函数
        return false;
    if (msg == NULL || !EQ_STR(msg->type, "NORMAL"))  // msg非正常值
        return false;

    af_Object *obj = *(af_Object **)(msg->msg);
    pushMacroFuncActivity(obj, env);
    gc_delReference(obj);
    freeMessage(msg);
    return true;
}

/*
 * 函数名: checkRunGC
 * 目标: 检查是否该运行gc, 若是则返回true并运行gc, 否则返回false
 */
static bool checkRunGC(af_Environment *env) {
    if (env->core->gc_run == grt_always ||
        env->core->gc_run == grt_count && env->core->gc_count >= env->core->gc_count_max) {
        gc_RunGC(env);
        return true;
    }
    return false;
}

/*
 * 函数名: iterCodeInit
 * 目标: 初始化activity和environment (若environment中未存在activity则通过code新增一个TopActivity, 否则沿用原activity)
 */
static bool iterCodeInit(af_Code *code, af_Environment *env) {
    if (env->activity == NULL && code == NULL || env->activity != NULL && code != NULL)
        return false;
    if (code != NULL && !addTopActivity(code, env))  // 初始化环境
        return false;
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
            pushMessageDown(makeERRORMessageFormate(LITERAL_ERROR, env, "Literal not found: %s.", code->element.data), env);
            return false;
        }

        return pushLiteralActivity(code, code->element.data, var->vn->obj, env);
    }

    /* 变量执行 */
    var = findVarFromVarList(code->element.data, env->activity->belong, env->activity->vsl);

    if (var == NULL) {
        pushMessageDown(makeERRORMessageFormate(VARIABLE_ERROR, env, "Variable not found: %s.", code->element.data), env);
        return false;
    }

    af_Object *obj = var->vn->obj;
    obj_isObjFunc *is_obj;
    obj_isInfixFunc *is_infix;

    if (code->prefix != getPrefix(V_QUOTE, env)) {
        if ((is_obj = findAPI("obj_isObjFunc", obj->data->api)) != NULL && is_obj(obj))
            return pushVariableActivity(code, var->vn->obj, env);  // 对象函数
        else if (env->activity->status != act_func_get && // 在act_func模式时关闭保护
                 (is_infix = findAPI("obj_isInfixFunc", obj->data->api)) != NULL && is_infix(obj)) {
            pushMessageDown(makeERRORMessageFormate(INFIX_PROTECT, env,
                                                    "Infix protect variable: %s.", code->element.data), env);
            return false;
        }
    }

    pushMessageDown(makeNORMALMessage(obj), env);
    env->activity->bt_next = env->activity->bt_next->next;
    printf("Get Variable %s : %p\n", code->element.data, obj);
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
 * 函数名: setRunVarSpaceList
 * 目标: 设置运行的变量空间
 * 仅在act_arg模式下, 允许运行变量空间设置为函数变量空间 (参数计算)
 */
static void setRunVarSpaceList(af_Environment *env) {
    if (env->activity->status == act_func_arg && env->activity->run_in_func && env->activity->func_var_list != NULL)
        env->activity->vsl = env->activity->func_var_list;
    else
        env->activity->vsl = env->activity->var_list;
}

/*
 * 函数名: runCodeBase
 * 目标: 获取代码类型, 调用(codeLiteral, codeElement, codeBlock)运行代码
 * 返回true- 表示执行无消息写入 msg_down (通常是无实质性代码运算)
 * 返回false-表示有消息写入 msg_down
 */
static bool runCodeBase(af_Environment *env) {
    switch (env->activity->bt_next->type) {
        case code_element:
            if (codeElement(env->activity->bt_next, env))
                return true;
            break;
        case code_block:
            if (codeBlock(env->activity->bt_next, env))
                return true;  // 若运行成功则跳转到下一次运行, 该步骤仅为设置Activity
        default:
            break;  // 错误
    }
    return false;
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
 * 函数名: runCode
 * 目标: 执行代码
 * *msg - 写入获取的执行信息
 * *run_code - 写入是否有代码运行
 * 返回-true  表示未获得msg (未进行实质性运算)
 * 返回-false 表示获得msg  (进行实质性运算)
 */
static bool runCode(af_Message **msg, bool *run_code, af_Environment *env) {
    if (env->activity->bt_next == NULL && env->activity->process_msg_first == 0) {  // 无代码运行, 并且非msg_first
        *run_code = false;
        *msg = NULL;
        return false;
    }

    if (env->activity->process_msg_first == 0) {
        if (env->activity->bt_next == NULL) {
            *run_code = false;
            *msg = NULL;
            return false;
        } else if (runCodeBase(env)) {
            *msg = NULL;
            return true;  // (该步骤仅为设置Activity, 无实际运算)
        }
    } else
        env->activity->process_msg_first--;

    *run_code = true;
    (*msg) = getTopMsg(env);

    switch (checkMsg((*msg), env)) {
        case 0:  // 不可处理的信号
            *msg = NULL;
            popActivity(false, NULL, env);  // 跳出当前activity
            return true;
        case -1:  // 非正常但可处理 [已经放回]
            (*msg) = NULL;
            break;
        case 1:  // 正常信号
            if (env->activity->return_first && env->activity->return_obj == NULL)  // 设置return_first
                env->activity->return_obj = *(af_Object **)(*msg)->msg;
            break;
        default:
            break;
    }
    return false;
}

/*
 * 函数名: checkNormalEnd
 * 目标: 检查act_normal是否运行到结尾 (若运行到结尾则返回true, 否则返回false)
 */
bool checkNormalEnd(af_Message *msg, af_Environment *env) {
    if (env->activity->bt_next == NULL) {
        if (setFuncActivityToNormal(env) == 0) {  // 已经没有下一步了
            if (checkMacro(msg, env))  // 检查是否宏函数
                return false;  // 继续执行
            checkLiteral(&msg, env);  // 检查是否字面量
            pushMessageDown(msg, env);
            return true;
        } else {
            gc_delReference(*(af_Object **) (msg->msg));  // msg->msg是一个指针, 这个指针的内容是一个af_Object *
            freeMessage(msg);
        }
    } else {
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

/*
 * 函数名: processMsg
 * 目标: 处理msg
 */
static void processMsg(af_Message *msg, bool run_code, af_Environment *env) {
    switch (env->activity->status) {
        case act_func_normal:
            if (!run_code)
                popActivity(false, makeERRORMessage(RUN_ERROR, NOT_CODE_INFO, env), env);
            else if (checkNormalEnd(msg, env))
                popActivity(true, NULL, env);  // 正常退出
            break;
        case act_func_get:
            if (!run_code)
                popActivity(false, makeERRORMessage(RUN_ERROR, NOT_CODE_INFO, env), env);
            else {
                af_Object *func = *(af_Object **)(msg->msg);  // func仍保留了msg的gc计数
                gc_delReference(func);  // 释放计数
                freeMessage(msg);
                if (!setFuncActivityToArg(func, env))
                    popActivity(false, NULL, env);
            }
            break;
        case act_func_arg: {
            if (!run_code || checkGetArgEnd(msg, env)) {  // 无参数设定或参数设定完成
                if (!setFuncActivityAddVar(env))
                    popActivity(false, NULL, env);
            }
            break;
        }
        default:
            break;
    }
}

/*
 * 函数名: iterCode
 * 目标: 运行代码 (代码可通过code参数传入, 或通过env->activity传入)
 */
bool iterCode(af_Code *code, af_Environment *env) {
    if (!iterCodeInit(code, env))
        return false;

    for (NULL; env->activity != NULL; ) {
        af_Message *msg = NULL;
        bool run_code = false;
        checkRunGC(env);

        if (env->activity->type == act_gc) {  // gc 模式
            if (env->activity->dl_next == NULL)
                popActivity(true, NULL, env);  // 结束运行
            else {
                printf("env->activity->dl_next.obj = %p, %d\n", env->activity->dl_next->obj->data, env->activity->dl_next->obj->data->gc.done_destruct);
                pushDestructActivity(env->activity->dl_next, env);
            }
        } else {
            setRunVarSpaceList(env);
            if (runCode(&msg, &run_code, env))
                continue;  // 若未获得msg (未进行实质性运算) 则再次运算
            processMsg(msg, run_code, env);
        }
    }
    return true;
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
        if (!iterCode(NULL, env))
            return false;
    }
    return false;
}
