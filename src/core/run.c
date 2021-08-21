#include "aFun.h"

#include "run.h"
#include "__env.h"
#include "__object.h"
#include "__var.h"
#include "__gc.h"
#include "__code.h"

static af_Code *codeVariable(af_Code *code, af_Environment *env) {
    af_Var *var = findVarFromVarList(code->variable.name, env->activity->var_list);
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
    return code->next;
}

static af_Code *codeLiteral(af_Code *code, af_Environment *env) {
    af_Message *msg;
    af_Object *obj = makeObject("Literal", 0, true, true, NULL, NULL, env);
    printf("Literal %s(%s) : %p\n", code->literal.func, code->literal.literal_data, obj);
    msg = makeMessage("NORMAL", sizeof(af_Object *));
    *((af_Object **)msg->msg) = obj;
    gc_addReference(obj);
    pushMessageDown(msg, env);
    return code->next;
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

static void popLastActivity(af_Message *msg, af_Environment *env){
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

static void codeBlock(af_Code *bt, af_Environment *env) {
    if (bt->prefix == env->core->prefix[B_EXEC] && bt->block.type == parentheses)  // 顺序执行, 返回尾项
        pushExecutionActivity(bt, false, env);
    else if (bt->prefix == env->core->prefix[B_EXEC_FIRST] && bt->block.type == brackets)  // 顺序执行, 返回首项
        pushExecutionActivity(bt, true, env);
    else {
        pushFuncActivity(env->activity->bt_next, env);
        if (bt->prefix == env->core->prefix[B_MUST_COMMON_ARG])
            env->activity->must_common_arg = true;
        else if (bt->prefix == env->core->prefix[B_NOT_STRICT])
            env->activity->not_strict = true;
    }
}

bool iterCode(af_Code *code, af_Environment *env) {
    if (!addTopActivity(code, env))
       return false;

    while (env->activity != NULL) {
        af_Message *msg;
        switch (env->activity->bt_next->type) {
            case literal:
                env->activity->bt_next = codeLiteral(env->activity->bt_next, env);
                break;
            case variable:
                env->activity->bt_next = codeVariable(env->activity->bt_next, env);
                break;
            case block:
                codeBlock(env->activity->bt_next, env);
                continue;  // 该步骤没有任何实质性运算
            default:
                break;  // 错误
        }

        if (env->activity->msg_down == NULL) {
            msg = makeMessage("ERROR-STR", 0);
        } else
            msg = getFirstMessage(env);

        if (!EQ_STR(msg->type, "NORMAL")) {
            pushMessageDown(msg, env);
            if (env->activity->status != act_normal || !checkInMsgType(msg->type, env)) {
                if (env->activity->return_obj != NULL)
                    gc_delReference(env->activity->return_obj);
                env->activity->return_obj = NULL;
                popLastActivity(NULL, env);  // msg 已经 push进去了
                continue;
            } else {
                env->activity->bt_next = NULL;
                // TODO-szh 切换函数
            }
        } else if (env->activity->return_first && env->activity->return_obj == NULL) {
            env->activity->return_obj = *(af_Object **)msg->msg;
            gc_addReference(env->activity->return_obj);
        }

        switch (env->activity->status) {
            case act_normal:
                if (env->activity->bt_next == NULL) { // 执行完成
                    popLastActivity(msg, env);
                } else {
                    gc_delReference(*(af_Object **)(msg->msg));  // msg->msg是一个指针, 这个指针的内容是一个af_Object *
                    freeMessage(msg);
                }
                break;
            case act_func: {
                af_Object *func = *(af_Object **)(msg->msg);  // func仍保留了msg的gc计数
                freeMessage(msg);
                setFuncActivityToArg(func, env);  // 该函数会设定bt_next到arg计算的bt上
                gc_delReference(func);  // 释放计数
                break;
            }
            case act_arg: {  // TODO-szh 暂时跳过参数设定
                setFuncActivityAddVar(NULL, true, false, NULL, env);
                setFuncActivityToNormal(env->activity->bt_next, env);
                gc_delReference(*(af_Object **)(msg->msg));  // 释放计数
                freeMessage(msg);
                break;
            }
            default:
                break;
        }

    }
    return true;
}