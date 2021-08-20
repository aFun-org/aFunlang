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
    } else {
        msg = makeMessage("ERROR-STR", 0);
        printf("Var not found: %s\n", code->variable.name);
    }

    pushMessageDown(msg, env);
    return code->next;
}

static af_Code *codeLiteral(af_Code *code, af_Environment *env) {
    af_Message *msg;
    af_Object *obj = makeObject("Literal", 0, true, true, NULL, NULL, env);
    printf("Literal %s : %s\n", code->literal.func, code->literal.literal_data);
    msg = makeMessage("NORMAL", sizeof(af_Object *));
    *((af_Object **)msg->msg) = obj;
    gc_addReference(obj);
    pushMessageDown(msg, env);
    return code->next;
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
            case block:  // TODO-szh 考虑前缀
                pushFuncActivity(env->activity->bt_next, env);
                continue;  // 该步骤没有任何实质性运算
            default:
                break;  // 错误
        }

        msg = getFirstMessage(env);  // TODO-szh 检查是否为 NORMAL

        switch (env->activity->status) {
            case act_normal:
                if (env->activity->bt_next == NULL) { // 执行完成
                    do {  // 如果返回一级后仍是执行完成则继续返回
                        if (env->activity->prev == NULL)
                            printf("top finished\n");
                        popActivity(msg, env);
                        msg = NULL;  // 随后几次执行popActivity时不需要压入新的msg
                    } while (env->activity != NULL && env->activity->bt_next == NULL);
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