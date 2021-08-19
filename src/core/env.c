#include "__env.h"

static af_Core *makeCore(void);
static void freeCore(af_Core *core);
static bool checkInheritAPI(af_ObjectData *od);
static void checkInherit(af_Inherit **ih, af_Object *obj);
static bool enableCore(af_Core *core);

static af_Activity *makeActivity(af_ByteCode *bt,bool new_vs, af_VarSpaceListNode *vsl, af_Object *belong);
static af_Activity *freeActivity(af_Activity *activity);
static void freeAllActivity(af_Activity *activity);

static af_Core *makeCore(void) {
    af_Core *core = calloc(sizeof(af_Core), 1);
    core->in_init = true;
    core->protect = makeVarSpace();
    return core;
}

static void freeCore(af_Core *core) {
    freeVarSpace(core->protect);  // 无论是否gc接管都释放
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

    core->in_init = false;
    return true;
}

static af_Activity *makeActivity(af_ByteCode *bt, bool new_vs, af_VarSpaceListNode *vsl, af_Object *belong) {
    af_Activity *activity = calloc(sizeof(af_Activity), 1);
    activity->bt = bt;
    activity->bt_start = bt;

    if (new_vs) {
        activity->var_list = pushNewVarList(vsl);
        activity->new_vs_count = 1;
    } else {
        activity->var_list = vsl;
        activity->new_vs_count = 0;
    }

    activity->belong = belong;
    return activity;
}

static af_Activity *freeActivity(af_Activity *activity) {
    af_Activity *prev = activity->prev;
    af_VarSpaceListNode *vs = activity->var_list;

    for (int i = activity->new_vs_count; i > 0; i--) {
        if (vs == NULL)  // 发生了错误
            break;
        vs = popLastVarList(vs);
    }

    freeAllMessage(activity->msg_up);  // msg转移后需要将对应成员设置为NULL
    freeAllMessage(activity->msg_down);
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
}

void pushMessageDown(af_Message *msg, af_Environment *env) {
    msg->next = env->activity->msg_down;
    env->activity->msg_down = msg;
}

af_Message *popMessageUp(char *type, af_Environment *env) {
    for (af_Message **pmsg = &env->activity->msg_up; *pmsg != NULL; pmsg = &((*pmsg)->next)) {
        if (EQ_STR((*pmsg)->type, type)) {
            af_Message *msg = *pmsg;
            *pmsg = msg->next;
            return msg;
        }
    }
    return NULL;
}

af_Message *popMessageDown(char *type, af_Environment *env) {
    for (af_Message **pmsg = &env->activity->msg_down; *pmsg != NULL; pmsg = &((*pmsg)->next)) {
        if (EQ_STR((*pmsg)->type, type)) {
            af_Message *msg = *pmsg;
            *pmsg = msg->next;
            return msg;
        }
    }
    return NULL;
}

af_Environment *makeEnvironment(void) {
    af_Environment *env = calloc(sizeof(af_Environment), 1);
    env->core = makeCore();
    return env;
}

bool enableEnvironment(af_ByteCode *bt, af_Environment *env) {
    if (!enableCore(env->core))
        return false;

    env->activity = makeActivity(bt, false, NULL, env->core->global);
    env->activity->new_vs_count = 2;
    env->activity->var_list = makeVarSpaceList(env->core->global->data->var_space);
    env->activity->var_list->next = makeVarSpaceList(env->core->protect);
    env->activity->is_top = true;  // 设置为最顶层
    return true;
}

void freeEnvironment(af_Environment *env) {
    freeCore(env->core);
    freeAllActivity(env->activity);
    free(env);
}

void pushActivity(af_ByteCode *bt, bool new_vs, af_VarSpaceListNode *vsl, af_Object *belong,
                 af_Environment *env) {
    af_Activity *activity = makeActivity(bt, new_vs, vsl, belong);
    activity->prev = env->activity;
    env->activity = activity;
}

void popActivity(af_Environment *env) {
    env->activity = freeActivity(env->activity);
}
