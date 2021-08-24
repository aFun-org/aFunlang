#include "aFun.h"

#include "run.h"
#include "__env.h"

/* Code 执行函数 */
static void codeVariable(af_Code *code, af_Environment *env);
static bool codeLiteral(af_Code *code, af_Environment *env);
static bool codeBlock(af_Code *code, af_Environment *env);

/* 工具函数 */
static bool checkInMsgType(char *type, af_Environment *env);
static void popLastActivity(af_Message *msg, af_Environment *env) ;

static void codeVariable(af_Code *code, af_Environment *env) {
    af_Var *var = findVarFromVarList(code->variable.name, env->activity->vsl);
    af_Message *msg;

    if (var != NULL) {
        af_Object *obj = var->vn->obj;
        msg = makeMessage("NORMAL", sizeof(af_Object *));
        *((af_Object **)msg->msg) = obj;
        gc_addReference(obj);
        printf("Get Variable %s : %p\n", code->variable.name, obj);
    } else {
        msg = makeMessage("ERROR-STR", 0);
        printf("Variable not found: %s\n", code->variable.name);
    }

    pushMessageDown(msg, env);
    env->activity->bt_next = env->activity->bt_next->next;
}

static bool codeLiteral(af_Code *code, af_Environment *env) {
    af_Var *var;

    if (code->literal.in_protect)
        var = findVarFromVarSpace(code->literal.func, env->core->protect);
    else
        var = findVarFromVarList(code->literal.func, env->activity->vsl);

    if (var == NULL) {
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        printf("Literal not found: %s\n", code->variable.name);
        return false;
    }

    return pushLiteralActivity(code, var->vn->obj, env);
}

static bool codeBlock(af_Code *code, af_Environment *env) {
    if (code->prefix == env->core->prefix[B_EXEC] && code->block.type == parentheses)  // 顺序执行, 返回尾项
        return pushExecutionActivity(code, false, env);
    else if (code->prefix == env->core->prefix[B_EXEC_FIRST] && code->block.type == brackets)  // 顺序执行, 返回首项
        return pushExecutionActivity(code, true, env);
    else
        return pushFuncActivity(env->activity->bt_next, env);
}

static bool checkInMsgType(char *type, af_Environment *env) {
    if (env->activity->body_next == NULL || env->activity->body_next->msg_type == NULL)
        return false;
    for (char *msg_type_node = *env->activity->body_next->msg_type; msg_type_node != NULL; msg_type_node++) {
        if (EQ_STR(type, msg_type_node))
            return true;
    }
    return false;
}

static void popLastActivity(af_Message *msg, af_Environment *env) {
    env->process_msg_first = true;

    do {  // 如果返回一级后仍是执行完成则继续返回
        if (env->activity->return_first) {
            if (msg != NULL) {
                gc_delReference(*(af_Object **)msg->msg);
                freeMessage(msg);
            }

            if (env->activity->return_obj == NULL)
                msg = makeMessage("ERROR-STR", 0);
            else {
                msg = makeMessage("NORMAL", sizeof(af_Object *));
                *(af_Object **)msg->msg = env->activity->return_obj;  // env->activity->return_obj本来就有一个gc_Reference
                env->activity->return_obj = NULL;
            }
        }
        popActivity(msg, env);
        msg = NULL;  // 随后几次执行popActivity时不需要压入新的msg
    } while (env->activity != NULL && env->activity->bt_next == NULL);
}

static bool checkLiteral(af_Message **msg, af_Environment *env) {
    if (!env->activity->is_literal)
        return true;

    af_Object *obj = *(af_Object **)((*msg)->msg);
    obj_literalSetting *func = findAPI("obj_literalSetting", obj->data->api);
    if (func == NULL) {
        gc_delReference(obj);
        freeMessage(*msg);
        *msg = makeMessage("ERROR-STR", 0);
        return false;
    }
    func(env->activity->bt_top->literal.literal_data, obj->data->data, obj, env);
    return true;
}

static bool checkMacro(af_Message *msg, af_Environment *env) {
    if (env->activity->fi == NULL || !env->activity->fi->is_macro)  // 非宏函数
        return false;

    af_Object *obj = *(af_Object **)(msg->msg);
    pushMacroFuncActivity(obj, env);
    gc_delReference(obj);
    freeMessage(msg);
    return true;
}

bool iterCode(af_Code *code, af_Environment *env) {
    env->process_msg_first = false;  // 优先处理msg而不是运行代码
    if (!addTopActivity(code, env))
        return false;

    while (env->activity != NULL) {
        af_Message *msg = NULL;
        bool run_code = false;

        if (env->activity->status == act_arg && env->activity->run_in_func && env->activity->func_var_list != NULL)
            env->activity->vsl = env->activity->func_var_list;
        else
            env->activity->vsl = env->activity->var_list;

        if (env->activity->bt_next != NULL) {
            run_code = true;
            if (!env->process_msg_first) {
                switch (env->activity->bt_next->type) {
                    case literal:
                        if (codeLiteral(env->activity->bt_next, env))
                            continue;  // 若运行成功则跳转到下一次运行, 该步骤仅为设置Activity
                        break;
                    case variable:
                        codeVariable(env->activity->bt_next, env);
                        break;
                    case block:
                        if (codeBlock(env->activity->bt_next, env))
                            continue;  // 若运行成功则跳转到下一次运行, 该步骤仅为设置Activity
                    default:
                        break;  // 错误
                }
            } else
                env->process_msg_first = false;

            if (env->activity->msg_down == NULL)  // 若未获得 msg
                msg = makeMessage("ERROR-STR", 0);
            else
                msg = getFirstMessage(env);

            if (!EQ_STR(msg->type, "NORMAL")) {  // 若msg为非正常值
                pushMessageDown(msg, env);  // msg不弹出
                if (env->activity->status != act_normal || !checkInMsgType(msg->type, env)) {  // 非normal模式, 或normal模式不匹配该msg
                    if (env->activity->return_obj != NULL)
                        gc_delReference(env->activity->return_obj);
                    env->activity->return_obj = NULL;
                    popLastActivity(NULL, env);  // msg 已经 push进去了
                    continue;
                }
            } else if (env->activity->return_first && env->activity->return_obj == NULL) {  // 设置return_first
                env->activity->return_obj = *(af_Object **)msg->msg;
                gc_addReference(env->activity->return_obj);
            }
        }

        switch (env->activity->status) {
            case act_normal:
                if (!run_code)
                    popLastActivity(makeMessage("ERROR-STR", 0), env);
                else if (env->activity->bt_next == NULL) { // 执行完成
                    switch (setFuncActivityToNormal(env)) {
                        case -1:  // 已经没有下一步了 (原msg不释放)
                            if (checkMacro(msg, env))  // 检查是否宏函数
                                break;  // 继续执行
                            checkLiteral(&msg, env);  // 检查是否字面量
                            popLastActivity(msg, env);
                            break;
                        case 0:  // 已经没有下一步了 (但原msg释放)
                            gc_delReference(*(af_Object **)(msg->msg));  // msg->msg是一个指针, 这个指针的内容是一个af_Object *
                            freeMessage(msg);

                            if (env->activity->msg_down == NULL)  // 检查是否有msg
                                msg = makeMessage("ERROR-STR", 0);
                            else {
                                msg = getFirstMessage(env);
                                if (checkMacro(msg, env))  // 检查是否宏函数
                                    break;  // 继续执行
                                checkLiteral(&msg, env);  // 检查是否字面量
                            }

                            popLastActivity(msg, env);
                            break;
                        default:
                        case 1:  // 继续运行
                            gc_delReference(*(af_Object **)(msg->msg));  // msg->msg是一个指针, 这个指针的内容是一个af_Object *
                            freeMessage(msg);
                            break;
                    }
                } else if (env->activity->bt_next->type == block && env->activity->bt_next->block.type == parentheses &&
                           env->activity->bt_next->prefix != env->core->prefix[B_EXEC]) {  // 类前缀调用
                    env->activity->parentheses_call = *(af_Object **)(msg->msg);
                    freeMessage(msg);
                } else {  // 继续运行
                    gc_delReference(*(af_Object **)(msg->msg));  // msg->msg是一个指针, 这个指针的内容是一个af_Object *
                    freeMessage(msg);
                }
                break;
            case act_func: {
                if (!run_code)
                    popLastActivity(makeMessage("ERROR-STR", 0), env);
                else {
                    af_Object *func = *(af_Object **)(msg->msg);  // func仍保留了msg的gc计数
                    gc_delReference(func);  // 释放计数
                    freeMessage(msg);
                    if (!setFuncActivityToArg(func, env))  // 该函数会设定bt_next到arg计算的bt上
                        popLastActivity(NULL, env);  // setFuncActivityToArg中已设置msg
                }
                break;
            }
            case act_arg: {
                if (!run_code) {  // 无参数设定
                    if (!setFuncActivityAddVar(true, false, env) || !setFuncActivityToNormal(env))
                        popLastActivity(NULL, env);
                } else {
                    env->activity->acl_next->result = *(af_Object **)(msg->msg);
                    freeMessage(msg);
                    if (env->activity->acl_next->next == NULL) { // 参数设定结束
                        if (!setFuncActivityAddVar(true, false, env) || !setFuncActivityToNormal(env))
                            popLastActivity(NULL, env);
                    } else {
                        env->activity->acl_next = env->activity->acl_next->next;
                        env->activity->bt_next = env->activity->acl_next->code;
                    }
                }
                break;
            }
            default:
                break;
        }

    }

    return true;
}