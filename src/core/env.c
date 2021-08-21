#include "__env.h"

static af_Core *makeCore(void);
static void freeCore(af_Core *core);
static bool checkInheritAPI(af_ObjectData *od);
static void checkInherit(af_Inherit **ih, af_Object *obj);
static bool enableCore(af_Core *core);

static af_Activity *makeActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                 af_VarSpaceListNode *vsl, af_Object *belong, af_Object *func);
static af_Activity *freeActivity(af_Activity *activity);
static void freeAllActivity(af_Activity *activity);

static af_EnvVar *makeEnvVar(char *name, char *data);
static af_EnvVar *freeEnvVar(af_EnvVar *var);
static void freeAllEnvVar(af_EnvVar *var);
static void freeEnvVarSpace(af_EnvVarSpace *evs);

static af_TopMsgProcess *makeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func);
static af_TopMsgProcess *freeTopMsgProcess(af_TopMsgProcess *mp);
static void freeAllTopMsgProcess(af_TopMsgProcess *mp);
static void *findTopMsgProcessFunc(char *type, af_Environment *env);
static void runTopMessageProcess(af_Environment *env);

static af_Core *makeCore(void) {
    af_Core *core = calloc(sizeof(af_Core), 1);
    core->in_init = true;
    core->protect = makeVarSpace();
    addVarSpaceGCByCore(core->protect, core);
    gc_addReference(core->protect);  // protect被外部引用, 由gc管理, 此处标记一个Reference
    return core;
}

static void freeCore(af_Core *core) {
    if (core->object != NULL)
        gc_delReference(core->object);
    if (core->global != NULL)
        gc_delReference(core->global);
    gc_delReference(core->protect);
    gc_freeAllValue(core);
    free(core);
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
            *ih = freeIherit(*ih);  // 释放该ih
        } else
            ih = &((*ih)->next);
    }
    *ih = makeIherit(obj);
}

static bool checkInheritAPI(af_ObjectData *od) {
    if (od->api != NULL)
        return true;

    if (!od->inherit_api)
        return false;

    if (od->iherit->obj->data->api == NULL && !checkInheritAPI(od->iherit->obj->data))
        return false;

    od->api = od->iherit->obj->data->api;
    return true;
}

static bool enableCore(af_Core *core) {
    af_Object *object = getBaseObjectFromCore("object", core);
    af_Object *global = getBaseObjectFromCore("global", core);

    if (global == NULL || global->belong != NULL)
        return false;  // global未找到 或其有属对象

    if (object == NULL || object->data->iherit != NULL || object->data->inherit_api || !object->data->allow_inherit)
        return false;  // object未找到 或其继承自其他对象 或其使用继承api 或其不可被继承

    core->global = global;
    core->object = object;
    addVarSpaceGCByCore(global->data->var_space, core);

    for (af_Object *obj = core->object; obj != NULL; obj = obj->gc.next) {
        if (obj == global)
            continue;
        if (obj->belong == NULL)
            obj->belong = global;
    }

    af_ObjectData *last = NULL;
    for (af_ObjectData *od = core->gc_ObjectData; od != NULL; od = od->gc.next) {
        last = od;
        if (od == object->data)
            continue;
        checkInherit(&od->iherit, object);
    }

    // 先创造的obj在后面, 因此倒着遍历, 先遍历到的obj依赖少, 可以减少checkInheritAPI递归的深度
    for (af_ObjectData *od = last; od != NULL; od = od->gc.prev) {
        if (od == object->data)
            continue;
        if(!checkInheritAPI(od))
            return false;
    }

    gc_addReference(object);
    gc_addReference(global);
    addVarSpaceGCByCore(global->data->var_space, core);  // global的vs是全局作用空间, 被外部引用, 所以由gc管理 (不需要要标记Reference, global已经标记了)
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
    gc_addReference(belong);
    if (func != NULL)
        gc_addReference(func);

    activity->bt_top = bt_top;
    activity->bt_start = bt_start;
    activity->bt_next = bt_start;

    activity->return_first = return_first;
    return activity;
}

static af_Activity *freeActivity(af_Activity *activity) {
    af_Activity *prev = activity->prev;
    af_VarSpaceListNode *vs = activity->var_list;
    af_Message *msg_up = activity->msg_up;

    gc_delReference(activity->belong);
    if (activity->func != NULL)
        gc_delReference(activity->func);

    if (activity->return_obj != NULL)
        gc_delReference(activity->return_obj);

    freeAllMessage(activity->msg_down);  // msg转移后需要将对应成员设置为NULL
    for (int i = activity->msg_up_count; i > 0; i--) {
        if (msg_up == NULL)  // 发生了错误
            break;
        msg_up = freeMessage(msg_up);
    }

    for (int i = activity->new_vs_count; i > 0; i--) {
        if (vs == NULL)  // 发生了错误
            break;
        vs = popLastVarList(vs);
    }

    free(activity);
    return prev;
}

static void freeAllActivity(af_Activity *activity) {
    while (activity != NULL)
        activity = freeActivity(activity);
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
    if (env->activity->new_vs_count == 0 || env->activity->msg_up == NULL)
        return NULL;

    af_Message *msg = env->activity->msg_up;
    env->activity->msg_up = msg->next;
    msg->next = NULL;
    env->activity->new_vs_count--;
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

void mp_NORMAL(af_Message *msg, af_Environment *env) {
    if (msg->msg == NULL || *(af_Object **)msg->msg == NULL) {
        printf("msg: %p error\n", msg->msg);
        return;
    }
    gc_delReference(*(af_Object **)msg->msg);
    printf("NORMAL Point: %p\n", *(af_Object **)msg->msg);
}

af_Environment *makeEnvironment(void) {
    af_Environment *env = calloc(sizeof(af_Environment), 1);
    DLC_SYMBOL(TopMsgProcessFunc) func = MAKE_SYMBOL(mp_NORMAL, TopMsgProcessFunc);
    env->core = makeCore();
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

bool pushExecutionActivity(af_Code *bt, bool return_first, af_Environment *env) {
    af_Code *next;
    if (!getCodeBlockNext(bt, &next))
        return false;

    af_Activity *activity = makeActivity(bt, bt->next, return_first, env->activity->msg_up,
                                         env->activity->var_list, env->activity->belong,
                                         env->activity->func);
    env->activity->bt_next = next;
    activity->prev = env->activity;
    env->activity = activity;
    env->activity->status = act_normal;
    return true;
}

bool pushFuncActivity(af_Code *bt, af_Environment *env) {
    af_Code *next;
    if (!getCodeBlockNext(bt, &next))
        return false;

    af_Activity *activity = makeActivity(bt, bt->next, false, env->activity->msg_up,
                                         env->activity->var_list, env->activity->belong,
                                         env->activity->func);
    env->activity->bt_next = next;
    activity->prev = env->activity;
    env->activity = activity;
    env->activity->status = act_func;
    return true;
}

bool setFuncActivityToArg(af_Object *func, af_Environment *env) {
    af_Object *belong = getBelongObject(func, env);
    gc_delReference(env->activity->belong);
    if (env->activity->func != NULL)
        gc_delReference(env->activity->func);
    gc_addReference(func);
    gc_addReference(belong);

    env->activity->func = func;
    env->activity->belong = belong;
    env->activity->status = act_arg;
    // TODO-szh 参数处理(计算)
    return true;
}

bool setFuncActivityAddVar(af_VarSpaceListNode *vsl, bool new_vsl, bool is_protect, char **msg_type, af_Environment *env) {
    if (env->activity->new_vs_count != 0 || !new_vsl && is_protect)
        return false;

    if (vsl != NULL)
        env->activity->var_list = vsl;
    if (new_vsl) {
        env->activity->var_list = pushNewVarList(env->activity->var_list);
        env->activity->new_vs_count = 1;
    }

    env->activity->msg_type = msg_type;
    // TODO-szh 参数处理(赋值)
    env->activity->var_list->vs->is_protect = is_protect;
    return true;
}

bool setFuncActivityToNormal(af_Code *bt, af_Environment *env) {
    env->activity->bt_start = bt;
    env->activity->bt_next = bt;
    env->activity->status = act_normal;
    return true;
}

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

void popActivity(af_Message *msg, af_Environment *env) {
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
