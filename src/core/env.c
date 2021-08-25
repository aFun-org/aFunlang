#include "__env.h"

/* Core 创建和释放 */
static af_Core *makeCore(enum GcRunTime grt);
static void freeCore(af_Core *core);

/* Core 初始化 */
static bool enableCore(af_Core *core);
static void checkInherit(af_Inherit **ih, af_Object *obj);

/* Activity 创建和释放 */
static af_Activity *makeActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                 af_VarSpaceListNode *vsl, af_Object *belong, af_Object *func);
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
static void *findTopMsgProcessFunc(char *type, af_Environment *env);
static void runTopMessageProcess(af_Environment *env);

/* LiteralData 创建与释放 */
static af_LiteralDataList *makeLiteralDataList(char *data);
static af_LiteralDataList *freeLiteralData_Pri(af_LiteralDataList *ld);

static af_Core *makeCore(enum GcRunTime grt) {
    af_Core *core = calloc(sizeof(af_Core), 1);
    core->in_init = true;
    core->protect = makeVarSpaceByCore(core);

    core->prefix[V_QUOTE] = '\'';
    core->prefix[B_EXEC] = '\'';
    core->prefix[B_EXEC_FIRST] = ',';
    core->gc_run = grt;
    core->gc_count_max = DEFAULT_GC_COUNT_MAX;
    return core;
}

static void freeCore(af_Core *core) {
    printGCByCode(core);
    gc_freeAllValue(core);
    free(core);
}

char setPrefix(size_t name, char prefix, af_Environment *env) {
    char old = env->core->prefix[name];
    if (name >= PREFIX_SIZE || prefix == NUL)
        return NUL;
    env->core->prefix[name] = prefix;
    return old;
}

char getPrefix(size_t name, af_Environment *env) {
    return env->core->prefix[name];
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
    af_Var *var = findVarFromVarSpace(name, core->protect);
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

static void checkInherit(af_Inherit **ih, af_Object *obj) {
    while (*ih != NULL) {
        if ((*ih)->obj->data == obj->data) {
            if ((*ih)->next == NULL && (*ih)->obj == obj)  // 最后一个就是obj
                return;  // 不需要任何更改
            *ih = freeInherit(*ih);  // 释放该ih
        } else
            ih = &((*ih)->next);
    }
    *ih = makeInherit(obj);
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
        checkInherit(&od->inherit, object);
    }

    core->global = global;
    core->object = object;
    core->protect->is_protect = true;
    core->in_init = false;
    return true;
}

static af_Activity *makeActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                 af_VarSpaceListNode *vsl, af_Object *belong, af_Object *func) {
    af_Activity *activity = calloc(sizeof(af_Activity), 1);
    activity->status = act_func;

    activity->msg_up = msg_up;
    activity->msg_up_count = 0;

    activity->var_list = vsl;
    activity->new_vs_count = 0;

    activity->belong = belong;
    activity->func = func;

    activity->bt_top = bt_top;
    activity->bt_start = bt_start;
    activity->bt_next = bt_start;

    activity->return_first = return_first;
    return activity;
}

static af_Activity *freeActivity(af_Activity *activity) {
    af_Activity *prev = activity->prev;

    freeAllMessage(activity->msg_down);  // msg转移后需要将对应成员设置为NULL
    freeMessageCount(activity->msg_up_count, activity->msg_up);

    // vsl 是引用自 var_list和func_var_list的 故不释放
    // func_var_list 是引用自函数的 故不释放
    freeVarSpaceListCount(activity->new_vs_count, activity->var_list);
    freeVarSpaceListCount(activity->macro_vs_count, activity->macro_vsl);

    freeAllArgCodeList(activity->acl_start);
    if (activity->fi != NULL)
        freeFuncInfo(activity->fi);
    freeAllLiteralData(activity->ld);
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

void mp_NORMAL(af_Message *msg, af_Environment *env) {
    if (msg->msg == NULL || *(af_Object **)msg->msg == NULL) {
        printf("msg: %p error\n", msg->msg);
        return;
    }
    gc_delReference(*(af_Object **)msg->msg);
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
    DLC_SYMBOL(TopMsgProcessFunc) func = MAKE_SYMBOL(mp_NORMAL, TopMsgProcessFunc);
    env->core = makeCore(grt);
    env->esv = makeEnvVarSpace();
    addTopMsgProcess("NORMAL", func, env);
    FREE_SYMBOL(func);
    return env;
}

bool addTopActivity(af_Code *code, af_Environment *env) {
    if (env->activity != NULL)
        return false;

    env->activity = makeActivity(NULL, code, false, NULL, NULL, env->core->global, NULL);
    env->activity->new_vs_count = 2;
    env->activity->var_list = makeVarSpaceList(env->core->global->data->var_space);
    env->activity->var_list->next = makeVarSpaceList(env->core->protect);
    env->activity->status = act_normal;
    return true;
}

bool enableEnvironment(af_Environment *env) {
    return enableCore(env->core);
}

void freeEnvironment(af_Environment *env) {
    freeCore(env->core);
    freeAllActivity(env->activity);
    freeEnvVarSpace(env->esv);
    freeAllTopMsgProcess(env->process);
    free(env);
}

void checkRunGC(af_Environment *env) {
    if (env->core->gc_run == grt_always ||
        env->core->gc_run == grt_count && env->core->gc_count >= env->core->gc_count_max) {
        gc_RunGC(env);
    }
}

bool addVarToProtectVarSpace(af_Var *var, af_Environment *env) {
    return addVarToVarSpace(var, env->core->protect);
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

static void *findTopMsgProcessFunc(char *type, af_Environment *env) {
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
    if (next == NULL && env->activity->body_next == NULL) {
        printf("Tail tone recursive optimization\n");
        clearActivity(env->activity);
        env->activity->bt_top = bt;
        if (!env->activity->return_first)  // 若原本就有设置 return_first 则没有在设置的必要了, 因为该执行不会被返回
            env->activity->return_first = return_first;
    } else {
        af_Activity *activity = makeActivity(bt, NULL, return_first, env->activity->msg_up,
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

    env->activity->status = act_normal;
    return true;
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

    if (bt->block.type == curly) {  // 大括号
        if (bt->block.elements == 0) {
            pushMessageDown(makeMessage("ERROR-STR", 0), env);
            return false;
        } else
            func = bt->next;
    } else if (bt->block.type == brackets) {  // 暂时不考虑中括号
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    } else
        func = NULL;  // 小括号则不在需要匹配

    env->activity->bt_next = next;

    newActivity(bt, next, false, env);
    env->activity->bt_start = func;
    env->activity->bt_next = func;

    env->activity->call_type = env->activity->bt_top->block.type;
    env->activity->status = act_func;
    if (env->activity->call_type == parentheses)  // 对于类前缀调用, 已经获得func的实际值了
        return setFuncActivityToArg(parentheses_call, env);
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
    newActivity(bt, bt->next, false, env);
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

bool setFuncActivityToArg(af_Object *func, af_Environment *env) {
    obj_funcGetArgCodeList *get_acl = findAPI("obj_funcGetArgCodeList", func->data->api);
    obj_funcGetVarList *get_var_list = findAPI("obj_funcGetVarList", func->data->api);

    if (get_var_list == NULL || get_acl == NULL) {
        pushMessageDown(makeMessage("ERROR-STR", 0), env);
        return false;
    }

    env->activity->func = func;
    env->activity->belong = getBelongObject(func, env);
    env->activity->status = act_arg;

    /* 遇到错误时 get_acl 和 get_var_list 要自行设定msg */
    if (!get_acl(&env->activity->acl_start, func, env->activity->bt_top, &env->activity->mark, env))  // 设置acl
        return false;
    if (!get_var_list(&env->activity->func_var_list, func, env->activity->mark, env))  // 设置 func_var_list
        return false;

    env->activity->acl_done = env->activity->acl_start;
    if (env->activity->acl_done != NULL)
        env->activity->bt_next = env->activity->acl_done->code;
    else
        env->activity->bt_next = NULL;
    return true;
}

bool setFuncActivityAddVar(af_Environment *env){
    obj_funcGetInfo *get_info = findAPI("obj_funcGetInfo", env->activity->func->data->api);
    obj_funcGetArgList *get_arg_list = findAPI("obj_funcGetArgList", env->activity->func->data->api);
    af_ArgList *al;

    if (get_info == NULL || get_arg_list == NULL) {
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
        env->activity->var_list = pushNewVarList(env->activity->var_list, env);
        env->activity->new_vs_count++;
    }

    env->activity->func_var_list = NULL;
    if (!get_arg_list(&al, env->activity->func, env->activity->acl_start, env->activity->mark, env))
        return false;
    runArgList(al, env->activity->var_list, env);
    freeAllArgList(al);

    if (env->activity->fi->embedded == protect_embedded)
        env->activity->var_list->vs->is_protect = true;

    int status = setFuncActivityToNormal(env);
    if (status == -1) {
        popActivity(makeMessage("ERROR-STR", 0), env);
        return false;
    } else if (status == 0)
        env->process_msg_first = true;  // 先不弹出activity, 通过act_normal处理msg
    return true;
}

int setFuncActivityToNormal(af_Environment *env){  // 获取函数的函数体
    env->activity->status = act_normal;
    env->activity->bt_next = NULL;

    if (env->activity->body_next == NULL)  // 已经没有下一步了 (原msg不释放)
        return -1;

    while (env->activity->body_next != NULL) {
        if (env->activity->body_next->type == func_body_c) {
            GET_SYMBOL(env->activity->body_next->c_func)(env->activity->mark, env);
            env->activity->body_next = env->activity->body_next->next;
        } else {
            env->activity->bt_start = env->activity->body_next->code;
            env->activity->bt_next = env->activity->body_next->code;
            env->activity->body_next = env->activity->body_next->next;
            return 1;  // 仍有下一步
        }
    }

    return 0;  // 没有下一步, 但运行了C函数 (原msg释放)
}

/*
 * 函数名: runTopMessageProcess
 * 目标: 运行顶层信息处理器
 */
static void runTopMessageProcess(af_Environment *env) {
    af_Message **pmsg = &env->activity->msg_down;
    while (*pmsg != NULL) {
        af_TopMsgProcess *mp = findTopMsgProcessFunc((*pmsg)->type, env);
        if (mp != NULL) {
            GET_SYMBOL(mp->func)(*pmsg, env);
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
    if (env->activity->func != NULL) {
        obj_funcFreeMask *func = findAPI("obj_funcFreeMask", env->activity->func->data->api);
        if (func != NULL)
            func(env->activity->mark);
        env->activity->mark = NULL;
    }
}

void popActivity(af_Message *msg, af_Environment *env) {
    env->process_msg_first = true;

    if (env->activity->return_first) {
        if (msg != NULL) {
            gc_delReference(*(af_Object **)msg->msg);
            freeMessage(msg);
        }

        if (env->activity->return_obj == NULL)
            msg = makeMessage("ERROR-STR", 0);
        else {
            msg = makeNORMALMessage(env->activity->return_obj);
            env->activity->return_obj = NULL;
        }
    }

    freeMark(env);

    if (env->activity->prev != NULL) {
        af_Message *new_msg;
        if (msg != NULL) {
            new_msg = msg;
            msg->next = env->activity->msg_down;
        } else
            new_msg = env->activity->msg_down;
        env->activity->msg_down = NULL;
        connectMessage(&new_msg, env->activity->prev->msg_down);
        env->activity->prev->msg_down = new_msg;
    } else {  // 到顶
        if (msg != NULL) {
            msg->next = env->activity->msg_down;
            env->activity->msg_down = msg;
        }
        runTopMessageProcess(env);
    }

    env->activity = freeActivity(env->activity);
}
