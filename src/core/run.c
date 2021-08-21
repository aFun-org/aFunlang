#include "aFun.h"

#include "run.h"
#include "__env.h"
#include "__object.h"
#include "__var.h"
#include "__gc.h"
#include "__code.h"

/* Code 执行函数 */
static void codeVariable(af_Code *code, af_Environment *env);
static void codeLiteral(af_Code *code, af_Environment *env);
static void codeBlock(af_Code *code, af_Environment *env);

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

static void codeLiteral(af_Code *code, af_Environment *env) {
    af_Object *obj = makeObject("Literal", 0, true, true, NULL, NULL, env);
    af_Message *msg = makeMessage("NORMAL", sizeof(af_Object *));
    *((af_Object **)msg->msg) = obj;
    gc_addReference(obj);
    pushMessageDown(msg, env);

    printf("Literal %s(%s) : %p\n", code->literal.func, code->literal.literal_data, obj);
    env->activity->bt_next = env->activity->bt_next->next;
}

static void codeBlock(af_Code *code, af_Environment *env) {
    if (code->prefix == env->core->prefix[B_EXEC] && code->block.type == parentheses)  // 顺序执行, 返回尾项
        pushExecutionActivity(code, false, env);
    else if (code->prefix == env->core->prefix[B_EXEC_FIRST] && code->block.type == brackets)  // 顺序执行, 返回首项
        pushExecutionActivity(code, true, env);
    else if (code->prefix == NUL) {
        pushFuncActivity(env->activity->bt_next, env);
        if (code->prefix == env->core->prefix[B_MUST_COMMON_ARG])
            env->activity->must_common_arg = true;
        else if (code->prefix == env->core->prefix[B_NOT_STRICT])
            env->activity->not_strict = true;
    } else
        pushMessageDown(makeMessage("ERROR-STR", 0), env);

}

static bool checkInMsgType(char *type, af_Environment *env) {
    if (env->activity->msg_type == NULL)
        return false;
    for (char *msg_type_node = *env->activity->msg_type; msg_type_node != NULL; msg_type_node++) {
        if (EQ_STR(type, msg_type_node))
            return true;
    }
    return false;
}

static void popLastActivity(af_Message *msg, af_Environment *env) {
    do {  // 如果返回一级后仍是执行完成则继续返回
        if (env->activity->prev == NULL)
            printf("top finished\n");
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

bool iterCode(af_Code *code, af_Environment *env) {
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
            if (!env->activity->in_call) {
                switch (env->activity->bt_next->type) {
                    case literal:
                        codeLiteral(env->activity->bt_next, env);
                        break;
                    case variable:
                        codeVariable(env->activity->bt_next, env);
                        break;
                    case block:
                        codeBlock(env->activity->bt_next, env);
                        continue;  // 该步骤没有任何实质性运算
                    default:
                        break;  // 错误
                }
            } else
                env->activity->in_call = false;

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
                if (!run_code) {
                    msg = makeMessage("ERROR-STR", 0);  // 无代码可运行
                    popLastActivity(msg, env);
                } else if (env->activity->bt_next == NULL) { // 执行完成
                    if (setFuncActivityToNormal(true, env))
                        goto run_continue;  // 继续运行
                    else
                        popLastActivity(msg, env);
                } else if (env->activity->bt_next->type == block && env->activity->bt_next->block.type == parentheses
                           && env->activity->bt_next->prefix == NUL) {  // 类前缀调用
                    env->activity->parentheses_call = *(af_Object **)(msg->msg);
                    freeMessage(msg);
                } else {
                    run_continue:
                    gc_delReference(*(af_Object **)(msg->msg));  // msg->msg是一个指针, 这个指针的内容是一个af_Object *
                    freeMessage(msg);
                }
                break;
            case act_func: {
                if (!run_code) {
                    msg = makeMessage("ERROR-STR", 0);  // 无代码可运行
                    popLastActivity(msg, env);
                } else {
                    af_Object *func = *(af_Object **)(msg->msg);  // func仍保留了msg的gc计数
                    freeMessage(msg);
                    setFuncActivityToArg(func, env);  // 该函数会设定bt_next到arg计算的bt上
                    gc_delReference(func);  // 释放计数
                }
                break;
            }
            case act_arg: {
                if (!run_code) {
                    act_arg_end:
                    setFuncActivityAddVar(true, false, NULL, env);
                    if (!setFuncActivityToNormal(true, env)) {
                        msg = makeMessage("ERROR-STR", 0);  // 无代码可运行
                        popLastActivity(msg, env);
                    }
                } else {
                    env->activity->acl_next->result = *(af_Object **)(msg->msg);
                    freeMessage(msg);
                    if (env->activity->acl_next->next == NULL)
                        goto act_arg_end;  // 参数设定结束

                    env->activity->acl_next = env->activity->acl_next->next;
                    env->activity->bt_next = env->activity->acl_next->code;
                }
                break;
            }
            default:
                break;
        }

    }

    return true;
}