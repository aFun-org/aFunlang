#include "aFun.h"
#include "__env.h"
#include "run.h"

/* Core 创建和释放 */
static af_Core *makeCore(enum GcRunTime grt);
static void freeCore(af_Environment *env);

/* Core 初始化 */
static bool enableCore(af_Core *core);
static bool checkInherit(af_Inherit **ih, af_Object *obj);

/* Activity 创建和释放 */
static af_Activity *makeActivity(af_Message *msg_up, af_VarSpaceListNode *vsl, af_Object *belong);
static af_Activity *makeFuncActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                     af_VarSpaceListNode *vsl, af_Object *belong, af_Object *func);
static af_Activity *makeGcActivity(gc_DestructList *dl, gc_DestructList **pdl, af_Environment *env);
static af_Activity *freeActivity(af_Activity *activity);
static void freeAllActivity(af_Activity *activity);
static void clearActivity(af_Activity *activity);

/* Activity 相关处理函数 */
static void freeMark(af_Environment *env);
static void newActivity(af_Code *bt, const af_Code *next, bool return_first, af_Environment *env);
static void freeMarkByActivity(af_Activity *activity);

/* 环境变量 创建与释放 */
static af_EnvVar *makeEnvVar(char *name, char *data);
static af_EnvVar *freeEnvVar(af_EnvVar *var);
static void freeAllEnvVar(af_EnvVar *var);
static void freeEnvVarSpace(af_EnvVarSpace *evs);

/* 顶层消息处理器 创建与释放 */
static af_TopMsgProcess *makeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func);
static af_TopMsgProcess *freeTopMsgProcess(af_TopMsgProcess *mp);
static void freeAllTopMsgProcess(af_TopMsgProcess *mp);

/* 顶层消息处理器 处理函数 */
static af_TopMsgProcess *findTopMsgProcessFunc(char *type, af_Environment *env);

/* LiteralData 创建与释放 */
static af_LiteralDataList *makeLiteralDataList(char *data);
static af_LiteralDataList *freeLiteralData_Pri(af_LiteralDataList *ld);

static af_Core *makeCore(enum GcRunTime grt) {
    af_Core *core = calloc(sizeof(af_Core), 1);
    core->in_init = true;
    core->protect = makeVarSpaceByCore(NULL, core);
    core->gc_run = grt;
    core->gc_count_max = DEFAULT_GC_COUNT_MAX;
    return core;
}

/*
 * 函数名: freeCore
 * 目标: 释放Core
 * 因为gc_freeAllValue需要env作为参数, 故使用env作为freeCore的参数
 */
static void freeCore(af_Environment *env) {
    printGCByCode(env->core);
    gc_freeAllValue(env);
    free(env->core);
}

char setPrefix(size_t name, char prefix, af_Environment *env) {
    if (name >= PREFIX_SIZE)
        return '-';  // 表示未获取到prefix (NUL在Code中表示无prefix)
    char *prefix_ = findEnvVar(ev_sys_prefix, env);
    if (prefix_ == NULL || strlen(prefix_) < PREFIX_SIZE)
        return '-';
    switch (name) {
        case V_QUOTE:
            if (prefix == NUL && strchr(LV_PREFIX, prefix) == NULL)
                prefix = '-';
            break;
        case B_EXEC:
        case B_EXEC_FIRST:
            if (prefix == NUL && strchr(B_PREFIX, prefix) == NULL)
                prefix = '-';
            break;
        default:
            break;
    }
    char old = prefix_[name];
    prefix_[name] = prefix;
    return old;
}

char getPrefix(size_t name, af_Environment *env) {
    if (name >= PREFIX_SIZE)
        return '-';  // 表示未获取到prefix (NUL在Code中表示无prefix)

    char *prefix = findEnvVar(ev_sys_prefix, env);
    if (prefix == NULL || strlen(prefix) < PREFIX_SIZE)
        return '-';
    return prefix[name];
}

af_VarSpace *getProtectVarSpace(af_Environment *env) {
    return env->core->protect;
}

/*
 * 函数名: getBaseObjectFromCore
 * 目标: 从VarSpace中获取一个量
 * 作用: 用于init初始化时在保护空间获得一些初始化对象
 */
af_Object *getBaseObjectFromCore(char *name, af_Core *core) {
    af_Var *var = findVarFromVarSpace(name, NULL, core->protect);
    if (var != NULL)
        return var->vn->obj;
    return NULL;
}

/*
 * 函数名: getBaseObject
 * 目标: getBaseObjectFromCore的对外接口
 */
af_Object *getBaseObject(char *name, af_Environment *env) {
    return getBaseObjectFromCore(name, env->core);
}

static bool checkInherit(af_Inherit **ih, af_Object *obj) {
    while (*ih != NULL) {
        if ((*ih)->obj->data == obj->data) {
            if ((*ih)->next == NULL && (*ih)->obj == obj)  // 最后一个就是obj
                return true;  // 不需要任何更改
            *ih = freeInherit(*ih);  // 释放该ih
        } else
            ih = &((*ih)->next);
    }
    *ih = makeInherit(obj);
    return (*ih == NULL) ? false : true;
}

static bool enableCore(af_Core *core) {
    af_Object *object = getBaseObjectFromCore("object", core);
    af_Object *global = getBaseObjectFromCore("global", core);

    if (global == NULL || global->belong != NULL)
        return false;  // global未找到 或其有属对象

    if (object == NULL || object->data->inherit != NULL || !object->data->allow_inherit)
        return false;  // object未找到 或其继承自其他对象 或其不可被继承

    for (af_Object *obj = core->gc_Object; obj != NULL; obj = obj->gc.next) {
        if (obj == global)
            continue;
        if (obj->belong == NULL)
            obj->belong = global;
    }

    for (af_ObjectData *od = core->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (od == object->data)
            continue;
        if (!checkInherit(&od->inherit, object))
            return false;
    }

    core->global = global;
    core->object = object;
    core->protect->is_protect = true;
    core->in_init = false;
    return true;
}

static af_Activity *makeActivity(af_Message *msg_up, af_VarSpaceListNode *vsl, af_Object *belong) {
    af_Activity *activity = calloc(sizeof(af_Activity), 1);
    activity->msg_up = msg_up;
    activity->msg_up_count = 0;
    activity->var_list = vsl;
    activity->new_vs_count = 0;
    activity->belong = belong;
    return activity;
}

static af_Activity *makeFuncActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                     af_VarSpaceListNode *vsl, af_Object *belong, af_Object *func) {
    af_Activity *activity = makeActivity(msg_up, vsl, belong);

    activity->type = act_func;
    activity->status = act_func_get;
    activity->func = func;
    activity->bt_top = bt_top;
    activity->bt_start = bt_start;
    activity->bt_next = bt_start;

    activity->return_first = return_first;
    return activity;
}

static af_Activity *makeGcActivity(gc_DestructList *dl, gc_DestructList **pdl, af_Environment *env) {
    af_Activity *activity = makeActivity(NULL, NULL, env->core->global);
    activity->type = act_gc;

    activity->var_list = makeVarSpaceList(getProtectVarSpace(env));
    activity->new_vs_count = 1;

    activity->dl = dl;
    activity->pdl = pdl;
    activity->dl_next = dl;
    return activity;
}

static af_Activity *freeActivity(af_Activity *activity) {
    af_Activity *prev = activity->prev;

    freeAllMessage(activity->msg_down);  // msg转移后需要将对应成员设置为NULL
    freeMessageCount(activity->msg_up_count, activity->msg_up);

    freeVarSpaceListCount(activity->new_vs_count, activity->var_list);

    if (activity->type == act_gc) {
        if (activity->dl != NULL)
            freeAllDestructList(activity->dl);
    } else {
        // vsl 是引用自 var_list和func_var_list的 故不释放
        // func_var_list 是引用自函数的 故不释放
        freeVarSpaceListCount(activity->macro_vs_count, activity->macro_vsl);

        freeAllArgCodeList(activity->acl_start);
        if (activity->fi != NULL)
            freeFuncInfo(activity->fi);
        freeAllLiteralData(activity->ld);
    }

    free(activity);
    return prev;
}

static void freeAllActivity(af_Activity *activity) {
    while (activity != NULL)
        activity = freeActivity(activity);
}

static void clearActivity(af_Activity *activity) {
    freeMarkByActivity(activity);
    freeVarSpaceListCount(activity->macro_vs_count, activity->macro_vsl);
    freeAllArgCodeList(activity->acl_start);
    if (activity->fi != NULL)
        freeFuncInfo(activity->fi);

    activity->func_var_list = NULL;
    activity->bt_top = NULL;
    activity->bt_start = NULL;
    activity->bt_next = NULL;

    activity->acl_start = NULL;
    activity->acl_done = NULL;
    activity->fi = NULL;
    activity->body_next = NULL;
}

/*
 * 函数名: makeLiteralDataList
 * 目标: 生成一个 af_LiteralDataList
 * 注意: char *data 要求传入一个已经被复制的data值
 * makeLiteralDataList是内部函数, 属于可控函数, 因此data在函数内部不再复制
 */
static af_LiteralDataList *makeLiteralDataList(char *data) {
    af_LiteralDataList *ld = calloc(sizeof(af_LiteralDataList), 1);
    ld->literal_data = data;
    return ld;
}

static af_LiteralDataList *freeLiteralData_Pri(af_LiteralDataList *ld) {
    af_LiteralDataList *next = ld->next;
    free(ld->literal_data);
    free(ld);
    return next;
}

void freeAllLiteralData(af_LiteralDataList *ld) {
    while (ld != NULL)
        ld = freeLiteralData_Pri(ld);
}

void pushLiteralData(char *data, af_Environment *env) {
    af_LiteralDataList *ld = makeLiteralDataList(data);
    ld->next = env->activity->ld;
    env->activity->ld = ld;
}

af_Message *makeMessage(char *type, size_t size) {
    af_Message *msg = calloc(sizeof(af_Message), 1);
    msg->type = strCopy(type);
    if (size != 0)
        msg->msg = calloc(size, 1);
    msg->size = size;
    return msg;
}

af_Message *freeMessage(af_Message *msg) {
    af_Message *next = msg->next;
    free(msg->type);
    free(msg->msg);
    free(msg);
    return next;
}

void freeAllMessage(af_Message *msg) {
    while (msg != NULL)
        msg = freeMessage(msg);
}

bool freeMessageCount(size_t count, af_Message *msg) {
    for (size_t i = count; i > 0; i--) {
        if (msg == NULL)  // 发生了错误
            return false;
        msg = freeMessage(msg);
    }
    return true;
}

void pushMessageUp(af_Message *msg, af_Environment *env) {
    msg->next = env->activity->msg_up;
    env->activity->msg_up = msg;
    env->activity->msg_up_count++;
}

void pushMessageDown(af_Message *msg, af_Environment *env) {
    msg->next = env->activity->msg_down;
    env->activity->msg_down = msg;
}

void *popMessageUpData(char *type, af_Environment *env) {
    for (af_Message **pmsg = &env->activity->msg_up; *pmsg != NULL; pmsg = &((*pmsg)->next)) {
        if (EQ_STR((*pmsg)->type, type))
            return (*pmsg)->msg;  // msg_up是只读的
    }
    return NULL;
}

af_Message *popMessageUp(af_Environment *env) {
    if (env->activity->msg_up_count == 0 || env->activity->msg_up == NULL)
        return NULL;

    af_Message *msg = env->activity->msg_up;
    env->activity->msg_up = msg->next;
    msg->next = NULL;
    env->activity->msg_up_count--;
    return msg;
}

/*
 * 函数名: getMessageData
 * 目标: 获取`msg`的数据, 对外API
 */
void *getMessageData(af_Message *msg) {
    return msg->msg;
}

af_Message *popMessageDown(char *type, af_Environment *env) {
    for (af_Message **pmsg = &env->activity->msg_down; *pmsg != NULL; pmsg = &((*pmsg)->next)) {
        if (EQ_STR((*pmsg)->type, type)) {
            af_Message *msg = *pmsg;
            *pmsg = msg->next;
            msg->next = NULL;
            return msg;
        }
    }
    return NULL;
}

af_Message *getFirstMessage(af_Environment *env) {
    af_Message *msg = env->activity->msg_down;
    env->activity->msg_down = msg->next;
    msg->next = NULL;
    return msg;
}

void connectMessage(af_Message **base, af_Message *msg) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = msg;
}

void mp_NORMAL(af_Message *msg, bool is_gc, af_Environment *env) {
    if (msg->msg == NULL || *(af_Object **)msg->msg == NULL) {
        printf("msg: %p error\n", msg->msg);
        return;
    }
    gc_delReference(*(af_Object **)msg->msg);
    if (!is_gc)
        printf("NORMAL Point: %p\n", *(af_Object **)msg->msg);
}

af_Message *makeNORMALMessage(af_Object *obj) {
    af_Message *msg = makeMessage("NORMAL", sizeof(af_Object *));
    *(af_Object **)msg->msg = obj;  // env->activity->return_obj本来就有一个gc_Reference
    gc_addReference(obj);
    return msg;
}

static af_EnvVar *makeEnvVar(char *name, char *data) {
    af_EnvVar *var = calloc(sizeof(af_EnvVar), 1);
    var->name = strCopy(name);
    var->data = strCopy(data);
    return var;
}

static af_EnvVar *freeEnvVar(af_EnvVar *var) {
    af_EnvVar *next = var->next;
    free(var->data);
    free(var->name);
    free(var);
    return next;
}

static void freeAllEnvVar(af_EnvVar *var) {
    while (var != NULL)
        var = freeEnvVar(var);
}

static af_EnvVarSpace *makeEnvVarSpace(void) {
    af_EnvVarSpace *esv = calloc(sizeof(af_EnvVarSpace), 1);
    return esv;
}

static void freeEnvVarSpace(af_EnvVarSpace *evs) {
    for (int i = 0; i < ENV_VAR_HASH_SIZE; i++)
        freeAllEnvVar(evs->var[i]);
    free(evs);
}

void setEnvVar(char *name, char *data, af_Environment *env) {
    time33_t index = time33(name) % ENV_VAR_HASH_SIZE;
    af_EnvVar **pvar = &env->esv->var[index];
    env->esv->count++;
    for (NULL; *pvar != NULL; pvar = &((*pvar)->next)) {
        if (EQ_STR((*pvar)->name, name)) {
            free((*pvar)->data);
            (*pvar)->data = strCopy(data);
            return;
        }
    }

    *pvar = makeEnvVar(name, data);
}

char *findEnvVar(char *name, af_Environment *env) {
    time33_t index = time33(name) % ENV_VAR_HASH_SIZE;
    af_EnvVar **pvar = &env->esv->var[index];

    for (NULL; *pvar != NULL; pvar = &((*pvar)->next)) {
        if (EQ_STR((*pvar)->name, name))
            return (*pvar)->data;
    }

    return NULL;
}

af_Environment *makeEnvironment(enum GcRunTime grt) {
    af_Environment *env = calloc(sizeof(af_Environment), 1);
    env->core = makeCore(grt);
    env->esv = makeEnvVarSpace();

    /* 设置默认prefix */
    char prefix[PREFIX_SIZE + 1] = "";
    prefix[V_QUOTE] = '\'';
    prefix[B_EXEC] = '\'';
    prefix[B_EXEC_FIRST] = ',';
    setEnvVar(ev_sys_prefix, prefix, env);

    /* 设置NORMAL顶级处理器 */
    DLC_SYMBOL(TopMsgProcessFunc) func = MAKE_SYMBOL(mp_NORMAL, TopMsgProcessFunc);
    addTopMsgProcess("NORMAL", func, env);
    FREE_SYMBOL(func);
    return env;
}

bool addTopActivity(af_Code *code, af_Environment *env) {
    if (env->activity != NULL)
        return false;

    env->activity = makeFuncActivity(NULL, code, false, NULL, NULL, env->core->global, NULL);
    env->activity->new_vs_count = 2;
    env->activity->var_list = makeVarSpaceList(env->core->global->data->var_space);
    env->activity->var_list->next = makeVarSpaceList(env->core->protect);
    env->activity->status = act_func_normal;
    return true;
}

bool enableEnvironment(af_Environment *env) {
    return enableCore(env->core);
}

void freeEnvironment(af_Environment *env) {
    if (!env->core->in_init)
        iterDestruct(10, env);
    freeCore(env);
    freeAllActivity(env->activity);
    freeEnvVarSpace(env->esv);
    freeAllTopMsgProcess(env->process);
    free(env);
}

bool addVarToProtectVarSpace(af_Var *var, af_Environment *env) {
    return addVarToVarSpace(var, NULL, env->core->protect);
}

static af_TopMsgProcess *makeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func) {
    af_TopMsgProcess *mp = calloc(sizeof(af_TopMsgProcess), 1);
    mp->type = strCopy(type);
    mp->func = COPY_SYMBOL(func, TopMsgProcessFunc);
    return mp;
}

static af_TopMsgProcess *freeTopMsgProcess(af_TopMsgProcess *mp) {
    af_TopMsgProcess *next = mp->next;
    free(mp->type);
    FREE_SYMBOL(mp->func);
    free(mp);
    return next;
}

static void freeAllTopMsgProcess(af_TopMsgProcess *mp) {
    while (mp != NULL)
        mp = freeTopMsgProcess(mp);
}

static af_TopMsgProcess *findTopMsgProcessFunc(char *type, af_Environment *env) {
    af_TopMsgProcess *mp = env->process;
    for (NULL; mp != NULL; mp = mp->next) {
        if (EQ_STR(type, mp->type))
            return mp;
    }
    return NULL;
}

void addTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func,
                      af_Environment *env) {
    af_TopMsgProcess *mp = makeTopMsgProcess(type, func);
    mp->next = env->process;
    env->process = mp;
}

bool changeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func,
                         af_Environment *env) {
    af_TopMsgProcess *mp = findTopMsgProcessFunc(type, env);
    if (mp == NULL)
        return false;
    FREE_SYMBOL(mp->func);
    mp->func = COPY_SYMBOL(func, TopMsgProcessFunc);
    return true;
}

static void newActivity(af_Code *bt, const af_Code *next, bool return_first, af_Environment *env){
    if (next == NULL && env->activity->body_next == NULL && env->activity->type == act_func) {
        printf("Tail tone recursive optimization\n");
        clearActivity(env->activity);
        env->activity->bt_top = bt;
        if (!env->activity->return_first)  // 若原本就有设置 return_first 则没有在设置的必要了, 因为该执行不会被返回
            env->activity->return_first = return_first;
    } else {
        af_Activity *activity = makeFuncActivity(bt, NULL, return_first, env->activity->msg_up,
                                                 env->activity->var_list, env->activity->belong,
                                                 env->activity->func);
        activity->prev = env->activity;
        env->activity = activity;
    }
}

bool pushExecutionActivity(af_Code *bt, bool return_first, af_Environment *env) {
    af_Code *next;
    if (!getCodeBlockNext(bt, &next)) {
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    }

    env->activity->bt_next = next;

    newActivity(bt, next, return_first, env);
    env->activity->bt_start = bt->next;
    env->activity->bt_next = bt->next;

    env->activity->status = act_func_normal;
    return true;
}

static bool isInfixFunc(af_Code *code, af_Environment *env) {
    if (code == NULL || code->type != variable || code->prefix == getPrefix(V_QUOTE, env))
        return false;

    af_Var *var = findVarFromVarList(code->variable.name, env->activity->belong, env->activity->var_list);
    if (var == NULL)
        return false;

    obj_isInfixFunc *func = findAPI("obj_isInfixFunc", var->vn->obj->data->api);
    if (func == NULL)
        return false;
    return func(var->vn->obj);
}

bool pushFuncActivity(af_Code *bt, af_Environment *env) {
    af_Code *next;
    af_Code *func;
    af_Object *parentheses_call = env->activity->parentheses_call;
    env->activity->parentheses_call = NULL;

    if (!getCodeBlockNext(bt, &next)) {
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    }

    switch (bt->block.type) {
        case curly:
            if (bt->block.elements == 0) {
                pushMessageDown(makeMessage("ERROR-STR", 0), env);
                return false;
            }
            func = bt->next;
            break;
        case brackets: {
            af_Code *code = bt->next;
            func = NULL;
            for (int i = 0; i < bt->block.elements; i++) {
                if (isInfixFunc(code, env)) {
                    func = code;
                    break;
                }
                if (!getCodeBlockNext(bt, &code))
                    break;
            }
            if (func == NULL) {
                pushMessageDown(makeMessage("ERROR-STR", 0), env);
                return false;
            }
            break;
        }
        case parentheses:
            func = NULL;  // 小括号则不在需要匹配
            break;
        default:
            break;
    }

    env->activity->bt_next = next;

    newActivity(bt, next, false, env);
    env->activity->bt_start = func;
    env->activity->bt_next = func;

    env->activity->call_type = env->activity->bt_top->block.type;
    env->activity->status = act_func_get;
    if (env->activity->call_type == parentheses) { // 对于类前缀调用, 已经获得func的实际值了
        if (parentheses_call == NULL) {
            pushMessageDown(makeMessage("ERROR-STR", 0), env);
            return false;
        }
        return setFuncActivityToArg(parentheses_call, env);
    }
    return true;
}

bool pushLiteralActivity(af_Code *bt, af_Object *func, af_Environment *env) {
    char *literal_data = strCopy(bt->literal.literal_data);  // newActivity可能会导致code和literal_data释放
    env->activity->bt_next = bt->next;

    /* 隐式调用不设置 bt_top */
    newActivity(NULL, bt->next, false, env);  // 如果原activity也是字面量, 则不进行尾调递归优化
    env->activity->is_literal = true;
    pushLiteralData(literal_data, env);
    return setFuncActivityToArg(func, env);
}

bool pushVariableActivity(af_Code *bt, af_Object *func, af_Environment *env) {
    env->activity->bt_next = bt->next;

    /* 隐式调用不设置 bt_top */
    newActivity(NULL, bt->next, false, env);
    return setFuncActivityToArg(func, env);
}

bool pushMacroFuncActivity(af_Object *func, af_Environment *env) {
    /* Macro是隐式调用, bt不移动 */
    /* 沿用activity */

    printf("Run macro\n");
    if (!freeVarSpaceListCount(env->activity->new_vs_count, env->activity->var_list)) { // 释放外部变量空间
        env->activity->new_vs_count = 0;
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    }

    env->activity->var_list = env->activity->macro_vsl;
    env->activity->new_vs_count = env->activity->macro_vs_count;
    env->activity->macro_vs_count = 0;

    clearActivity(env->activity); /* 隐式调用不设置 bt_top */
    return setFuncActivityToArg(func, env);
}

void pushGCActivity(gc_DestructList *dl, gc_DestructList **pdl, af_Environment *env) {
    for (af_Activity *tmp = env->activity; tmp != NULL; tmp = tmp->prev) {
        if (tmp->type == act_gc) {
            *(tmp->pdl) = dl;
            tmp->pdl = pdl;
            if (tmp->dl_next == NULL)  // 原dl_next已经运行到末端
                tmp->dl_next = dl;
            return;
        }
    }

    /* gc Activity 可能创建为顶层 activity, 故信息不能继承上一级(可能没有上一级) */
    af_Activity *activity = makeGcActivity(dl, pdl, env);

    activity->prev = env->activity;
    env->activity = activity;
}

bool pushDestructActivity(gc_DestructList *dl, af_Environment *env) {
    env->activity->dl_next = dl->next;

    /* 隐式调用不设置 bt_top */
    af_Activity *activity = makeFuncActivity(NULL, NULL, false, env->activity->msg_up,
                                             env->activity->var_list, env->activity->belong, NULL);
    activity->prev = env->activity;
    env->activity = activity;
    return setFuncActivityToArg(dl->func, env);
}

void setArgCodeListToActivity(af_ArgCodeList *acl, af_Environment *env) {
    if (acl != NULL) {
        env->activity->bt_start = acl->code;
        env->activity->bt_next = acl->code;
        env->activity->run_in_func = acl->run_in_func;
    } else {
        env->activity->bt_start = NULL;
        env->activity->bt_next = NULL;
        env->activity->run_in_func = false;
    }
}

bool setFuncActivityToArg(af_Object *func, af_Environment *env) {
    obj_funcGetArgCodeList *get_acl = findAPI("obj_funcGetArgCodeList", func->data->api);
    obj_funcGetVarList *get_var_list = findAPI("obj_funcGetVarList", func->data->api);

    if (get_var_list == NULL) {
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    }

    env->activity->func = func;
    env->activity->belong = getBelongObject(func);
    env->activity->status = act_func_arg;

    /* 遇到错误时 get_acl 和 get_var_list 要自行设定msg */
    if (get_acl != NULL) {
        if (!get_acl(&env->activity->acl_start, func, env->activity->bt_top, &env->activity->mark, env))  // 设置acl
            return false;
    } else
        env->activity->acl_start = NULL;

    if (!get_var_list(&env->activity->func_var_list, func, env->activity->mark, env))  // 设置 func_var_list
        return false;

    env->activity->acl_done = env->activity->acl_start;
    setArgCodeListToActivity(env->activity->acl_start, env);
    return true;
}

bool setFuncActivityAddVar(af_Environment *env){
    obj_funcGetInfo *get_info = findAPI("obj_funcGetInfo", env->activity->func->data->api);
    obj_funcGetArgList *get_arg_list = findAPI("obj_funcGetArgList", env->activity->func->data->api);

    if (get_info == NULL) {
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    }

    if (!get_info(&env->activity->fi, env->activity->func, env->activity->bt_top, env->activity->mark, env))
        return false;
    if (env->activity->fi == NULL) {
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    }
    if (env->activity->fi->scope == super_pure_scope && env->activity->fi->scope == super_embedded) {
        /* 超纯函数和超内嵌函数不得搭配使用 */
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    }
    env->activity->body_next = env->activity->fi->body;

    if (env->activity->fi->is_macro) {  // 是宏函数则保存变量空间
        env->activity->macro_vsl = env->activity->var_list;
        env->activity->macro_vs_count = env->activity->new_vs_count;
    } else if (env->activity->fi->scope != inline_scope) {  // 非内联函数, 释放外部变量空间
        if (!freeVarSpaceListCount(env->activity->new_vs_count, env->activity->var_list)) {
            pushMessageDown(makeMessage("ERROR-STR", 0), env);  // 释放失败
            return false;
        }
    }

    if (env->activity->fi->scope == normal_scope) {  // 使用函数变量空间
        env->activity->var_list = env->activity->func_var_list;
        env->activity->new_vs_count = 0;
    } else if (env->activity->fi->scope == pure_scope) {  // 纯函数只有 protect 变量空间
        env->activity->var_list = makeVarSpaceList(env->core->protect);
        env->activity->new_vs_count = 0;
    } else if (env->activity->fi->scope == super_pure_scope) {  // 超纯函数没有变量空间, 因此不得为超内嵌函数(否则var_list就为NULL了)
        env->activity->var_list = NULL;
        env->activity->new_vs_count = 0;
    }

    if (env->activity->fi->embedded != super_embedded) {  // 不是超内嵌函数则引入一层新的变量空间
        /* 新层的变量空间应该属于belong而不是func */
        env->activity->var_list = pushNewVarList(env->activity->belong, env->activity->var_list, env);
        env->activity->new_vs_count++;
    }

    env->activity->func_var_list = NULL;

    if (env->activity->fi->var_this && env->activity->belong != NULL) {
        if (!makeVarToVarSpaceList("this", 3, 3, 3, env->activity->belong,
                                   env->activity->var_list, env->activity->belong, env)) {
            pushMessageDown(makeMessage("ERROR-STR", 0), env);
            return false;
        }
    }

    if (env->activity->fi->var_func && env->activity->func != NULL) {
        if (!makeVarToVarSpaceList("func", 3, 3, 3, env->activity->func,
                                   env->activity->var_list, env->activity->belong, env)) {
            pushMessageDown(makeMessage("ERROR-STR", 0), env);
            return false;
        }
    }

    if (get_arg_list != NULL) {
        af_ArgList *al;
        if (!get_arg_list(&al, env->activity->func, env->activity->acl_start, env->activity->mark, env)) {
            pushMessageDown(makeMessage("ERROR-STR", 0), env);
            return false;
        }
        runArgList(al, env->activity->var_list, env);
        freeAllArgList(al);
    }

    if (env->activity->fi->embedded == protect_embedded)
        env->activity->var_list->vs->is_protect = true;

    if (setFuncActivityToNormal(env) == 0)
        return false;
    return true;
}

/*
 * 函数名: setFuncActivityToNormal
 * 目标: 获取下一步需要运行的结果
 * 返回  (0) 表示无下一步
 * 返回 (-1) 表示运行C函数, 并且设置了 process_msg_first
 * 返回  (1) 表示下一步运动Code
 * 返回  (2) 表示遇到未被替换的动态代码块
 */
int setFuncActivityToNormal(af_Environment *env){  // 获取函数的函数体
    af_FuncBody *body = env->activity->body_next;
    env->activity->status = act_func_normal;
    env->activity->bt_next = NULL;

    if (body == NULL)  // 已经没有下一步了 (原msg不释放)
        return 0;

    env->activity->body_next = body->next;
    switch (body->type) {
        case func_body_c: {
            af_FuncBody *new;
            new = GET_SYMBOL(body->c_func)(env->activity->mark, env);
            env->activity->process_msg_first++;  // 处理C函数通过msg_down返回的结果
            pushDynamicFuncBody(new, body);
            env->activity->body_next = body->next;  // 添加新元素后要重新设定body_next的位置
            return -1;
        }
        case func_body_code:
            env->activity->bt_start = body->code;
            env->activity->bt_next = body->code;
            return 1;
        default:
        case func_body_dynamic:
            pushMessageDown(makeMessage("ERROR-STR", 0), env);
            env->activity->process_msg_first++;  // 处理C函数通过msg_down返回的结果
            return 2;
    }
}

/*
 * 函数名: runTopMessageProcess
 * 目标: 运行顶层信息处理器
 */
void runTopMessageProcess(bool is_gc, af_Environment *env) {
    af_Message **pmsg = &env->activity->msg_down;
    while (*pmsg != NULL) {
        af_TopMsgProcess *mp = findTopMsgProcessFunc((*pmsg)->type, env);
        if (mp != NULL) {
            GET_SYMBOL(mp->func)(*pmsg, is_gc, env);
            *pmsg = freeMessage(*pmsg);
        } else
            pmsg = &((*pmsg)->next);
    }
}

static void freeMarkByActivity(af_Activity *activity) {
    if (activity->func != NULL) {
        obj_funcFreeMask *func = findAPI("obj_funcFreeMask", activity->func->data->api);
        if (func != NULL)
            func(activity->mark);
        activity->mark = NULL;
    }
}

static void freeMark(af_Environment *env) {
    if (env->activity->type == act_func && env->activity->func != NULL) {
        obj_funcFreeMask *func = findAPI("obj_funcFreeMask", env->activity->func->data->api);
        if (func != NULL)
            func(env->activity->mark);
        env->activity->mark = NULL;
    }
}

void popActivity(af_Message *msg, af_Environment *env) {
    if (env->activity->type == act_func) {
        if (msg != NULL && env->activity->return_first) {
            if (EQ_STR(msg->type, "NORMAL")) {
                gc_delReference(*(af_Object **) msg->msg);
                freeMessage(msg);
                msg = NULL;
            }
        } else if (env->activity->return_first) {  // msg == NULL
            if (env->activity->msg_down != NULL && EQ_STR(env->activity->msg_down->type, "NORMAL")) {
                af_Message *tmp = getFirstMessage(env);
                gc_delReference(*(af_Object **) (tmp->msg));
                freeMessage(tmp);
            }
        }

        if (msg == NULL && env->activity->return_first) {
            if (env->activity->return_obj == NULL)
                msg = makeMessage("ERROR-STR", 0);
            else
                msg = makeNORMALMessage(env->activity->return_obj);
        }
    }

    if (msg != NULL)
        pushMessageDown(msg, env);

    freeMark(env);

    if (env->activity->prev == NULL || env->activity->type == act_gc)  // 顶层或gc层
        runTopMessageProcess((env->activity->type == act_gc), env);
    else {
        connectMessage(&(env->activity->msg_down), env->activity->prev->msg_down);
        env->activity->prev->msg_down = env->activity->msg_down;
        env->activity->msg_down = NULL;
        env->activity->prev->process_msg_first++;  // 优先处理通过msg_down返回的结果
    }

    env->activity = freeActivity(env->activity);
}
