#include "__object.h"
#include "__env.h"
#include "tool.h"

/* ObjectData 创建与释放 */
static af_ObjectData * makeObjectData_Pri(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Object *base_obj,
                                          af_Environment *env);
static af_Object *makeObject_Pri(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Environment *env);

/* ObjectData API 创建与释放 */
static af_ObjectAPINode *makeObjectAPINode(DLC_SYMBOL(objectAPIFunc) func, char *api_name);
static af_ObjectAPINode *freeObjectAPINode(af_ObjectAPINode *apin);
static void freeAllObjectAPINode(af_ObjectAPINode *apin);

/* ObjectData API 管理函数 */
static af_ObjectAPINode *findObjectDataAPINode(char *api_name, af_ObjectData *od);
static int addAPIToObjectData(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_ObjectData *od);

/*
 * 函数名: makeObjectData_Pri
 * 目标: 创建ObjectData
 * 注意: af_ObjectData不是对外开放的结构体
 * 注意: api不能为NULL
 */
static af_ObjectData * makeObjectData_Pri(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Object *base_obj,
                                          af_Environment *env){
    af_ObjectData *od = calloc(1, sizeof(af_ObjectData));
    od->base = base_obj;
    base_obj->data = od;
    od->id = strCopy(id == NULL ? "Unknow" : id);

    od->api = api;
    od->free_api = free_api;
    od->allow_inherit = allow_inherit;

    od->var_space = makeVarSpace(base_obj, 3, 2, 0, env);
    od->inherit = NULL;

    obj_getDataSize *func = findAPI("obj_getDataSize", api);
    obj_initData *init = findAPI("obj_initData", api);
    if (func != NULL)
        od->size = func(id, base_obj);
    else
        od->size = 0;

    if (od->size != 0) {
        od->data = calloc(1, od->size);
        if (init != NULL)
            init(id, base_obj, od->data, env);
    }

    gc_addObjectData(od, env);
    return od;
}

static af_Object *makeObject_Pri(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Environment *env){
    af_Object *obj = calloc(1, sizeof(af_Object));
    obj->belong = NULL;
    makeObjectData_Pri(id, free_api, api, allow_inherit, obj, env);
    gc_addObject(obj, env);
    return obj;
}

/*
 * 函数名: 创建一个object
 * 目标: 生成Object和ObjectData, 并且添加到gc链表中
 * 若处于初始化模式, 则belong, inherit等可以设置为NULL, 由后期统一填上
 */
af_Object *makeObject(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Object *belong,
                      af_Inherit *inherit, af_Environment *env) {
    if (api == NULL)
        return NULL;

    af_Inherit *ih = NULL;
    if (inherit != NULL)
        ih = inherit;
    else if (env->core->global != NULL)  // init模式生成: global
        ih = makeInherit(env->core->global);
    else if (env->core->status != core_creat)
        return NULL;

    if (belong == NULL) {
        if (env->activity != NULL)
            belong = env->activity->belong;
        else if (env->core->status == core_init)  // init模式生成: global
            belong = env->core->global;
        else if (env->core->status != core_creat)  // 只有creat可以使用belong=NULL
            return NULL;
    }

    af_Object *obj = makeObject_Pri(id, free_api, api, allow_inherit, env);

    obj->belong = belong;
    obj->data->inherit = ih;
    return obj;
}

/*
 * 函数名: freeObjectData
 * 目标: 释放ObjectData, 仅GC函数可用
 * 对外API中, 创建对象的基本单位都是af_Object, 无法直接操控af_ObjectData
 */
void freeObjectData(af_ObjectData *od, af_Environment *env) {
    if (od->size != 0) {
        obj_destructData *func = findAPI("obj_destructData", od->api);
        if (func != NULL)
            func(od->id, od->base, od->data, env);
    }

    free(od->id);
    free(od->data);
    if (od->free_api)
        freeObjectAPI(od->api);
    freeAllInherit(od->inherit);
    GC_FREE_EXCHANGE(od, ObjectData, env->core);
    free(od);
}

void freeObject(af_Object *obj, af_Environment *env) {
    GC_FREE_EXCHANGE(obj, Object, env->core);
    free(obj);
}

void freeObjectByCore(af_Object *obj, af_Core *core) {
    GC_FREE_EXCHANGE(obj, Object, core);
    free(obj);
}

void *getObjectData(af_Object *obj) {
    return obj->data->data;
}

af_Object *getBelongObject(af_Object *object){
    if (object->belong == NULL)
        return object;
    return object->belong;
}

af_Inherit *makeInherit(af_Object *obj) {
    if (!obj->data->allow_inherit)
        return NULL;

    obj_getShareVarSpace *func = findAPI("obj_getShareVarSpace", obj->data->api);
    af_VarSpace *vs = NULL;
    if (func == NULL || (vs = func(obj->data->id, obj)) == NULL)
        return NULL;

    af_Inherit *ih = calloc(1, sizeof(af_Inherit));
    ih->vs = vs;
    ih->obj = obj;  // 调用API获取vs
    return ih;
}

af_Inherit *pushInherit(af_Inherit **base, af_Inherit *new) {
    while ((*base) != NULL)
        base = &((*base)->next);
    *base = new;
    while (new != NULL && new->next != NULL)
        new = new->next;
    return new;
}

static af_Inherit *freeInherit(af_Inherit *ih) {
    af_Inherit *next = ih->next;  // vs一定是被gc托管的
    free(ih);
    return next;
}

void freeAllInherit(af_Inherit *ih) {
    while (ih != NULL)
        ih = freeInherit(ih);
}

bool checkPosterity(af_Object *base, af_Object *posterity) {
    for (af_Inherit *ih = base->data->inherit; ih != NULL; ih = ih->next) {
        if (ih->obj->data == posterity->data)
            return true;
    }
    return false;
}

static af_ObjectAPINode *makeObjectAPINode(DLC_SYMBOL(objectAPIFunc) func, char *api_name) {
    if (func == NULL)
        return NULL;

    af_ObjectAPINode *apin = calloc(1, sizeof(af_ObjectAPINode));
    apin->api = COPY_SYMBOL(func, objectAPIFunc);
    apin->name = strCopy(api_name);
    return apin;
}

static af_ObjectAPINode *freeObjectAPINode(af_ObjectAPINode *apin) {
    af_ObjectAPINode *next = apin->next;
    FREE_SYMBOL(apin->api);
    free(apin->name);
    free(apin);
    return next;
}

static void freeAllObjectAPINode(af_ObjectAPINode *apin) {
    while (apin != NULL)
        apin = freeObjectAPINode(apin);
}

af_ObjectAPI *makeObjectAPI(void) {
    af_ObjectAPI *api = calloc(1, sizeof(af_ObjectAPI));
    return api;
}

void freeObjectAPI(af_ObjectAPI *api) {
    for (int i = 0; i < API_HASHTABLE_SIZE; i++)
        freeAllObjectAPINode(api->node[i]);
    free(api);
}

/*
 * 函数名: addAPIToObjectData
 * 目标: 从DLC中获取函数并写入api
 * 若已存在api则返回0且不作修改
 * 若dlc中不存在指定函数则返回-1且不作修改
 * 操作成功返回1
 */
int addAPI(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_ObjectAPI *api) {
    time33_t index = time33(api_name) % API_HASHTABLE_SIZE;
    af_ObjectAPINode **pNode = &api->node[index];

    for (NULL; *pNode != NULL; pNode = &((*pNode)->next)) {
        if (EQ_STR((*pNode)->name, api_name))
            return 0;
    }

    *pNode = makeObjectAPINode(func, api_name);
    api->count++;
    return *pNode == NULL ? -1 : 1;
}

void *findAPI(char *api_name, af_ObjectAPI *api) {
    time33_t index = time33(api_name) % API_HASHTABLE_SIZE;
    for (af_ObjectAPINode *node = api->node[index]; node != NULL; node = node->next) {
        if (EQ_STR(node->name, api_name))
            return GET_SYMBOL(node->api);
    }
    return NULL;
}

/*
 * 函数名: addAPIToObjectData
 * 目标: 从DLC中获取函数并写入api
 * 若已存在api则返回0且不作修改
 * 若dlc中不存在指定函数则返回-1且不作修改
 * 操作成功返回1
 */
static int addAPIToObjectData(DLC_SYMBOL(objectAPIFunc) func, char *api_name,
                              af_ObjectData *od) {
    time33_t index = time33(api_name) % API_HASHTABLE_SIZE;
    af_ObjectAPINode **pNode = &od->api->node[index];

    for (NULL; *pNode != NULL; pNode = &((*pNode)->next)) {
        if (EQ_STR((*pNode)->name, api_name))
            return 0;
    }

    *pNode = makeObjectAPINode(func, api_name);
    od->api->count++;
    return *pNode == NULL ? -1 : 1;
}

static af_ObjectAPINode *findObjectDataAPINode(char *api_name, af_ObjectData *od) {
    time33_t index = time33(api_name) % API_HASHTABLE_SIZE;
    for (af_ObjectAPINode *node = od->api->node[index]; node != NULL; node = node->next) {
        if (EQ_STR(node->name, api_name))
            return node;
    }
    return NULL;
}

/*
 * 函数名: findObjectDataAPINode
 * 目标: 从DLC中获取函数并写入Object的API
 */
int addAPIToObject(DLC_SYMBOL(objectAPIFunc) func, char *api_name,
                   af_Object *obj) {
    return addAPIToObjectData(func, api_name, obj->data);
}

/*
 * 函数名: findObjectAPI
 * 目标: 从Object中获取指定api的函数指针
 */
void *findObjectAPI(char *api_name, af_Object *obj) {
    af_ObjectAPINode *node = findObjectDataAPINode(api_name, obj->data);
    if (node == NULL)
        return NULL;
    return GET_SYMBOL(node->api);
}

af_Object *findObjectAttributes(char *name, af_Object *visitor, af_Object *obj) {
    af_Var *var = findVarFromVarSpace(name, visitor, obj->data->var_space);

    if (var != NULL)
        return var->vn->obj;

    for (af_Inherit *ih = obj->data->inherit; ih != NULL; ih = ih->next) {
        var = findVarFromVarSpace(name, visitor, ih->vs);  // 搜索共享变量空间
        if (var != NULL)
            return var->vn->obj;
    }

    return NULL;
}

bool setObjectAttributes(char *name, char p_self, char p_posterity, char p_external, af_Object *attributes,
                         af_Object *obj, af_Object *visitor, af_Environment *env){
    return makeVarToVarSpace(name, p_self, p_posterity, p_external, attributes, obj->data->var_space, visitor, env);
}

af_Object *findObjectAttributesByObjectData(char *name, af_Object *visitor, af_ObjectData *od) {
    af_Var *var = findVarFromVarSpace(name, visitor, od->var_space);

    if (var != NULL)
        return var->vn->obj;

    for (af_Inherit *ih = od->inherit; ih != NULL; ih = ih->next) {
        var = findVarFromVarSpace(name, visitor, ih->vs);  // 搜索共享变量空间
        if (var != NULL)
            return var->vn->obj;
    }

    return NULL;
}

char *getObjectID(af_Object *obj) {
    return obj->data->id;
}

af_ObjectAPI *getObjectAPI(af_Object *obj) {
    return obj->data->api;
}

af_Inherit *getObjectInherit(af_Object *obj) {
    return obj->data->inherit;
}

af_Inherit *getInheritNext(af_Inherit *ih) {
    return ih->next;
}

af_Object *getInheritObject(af_Inherit *ih) {
    return ih->obj;
}

af_VarSpace *getInheritVarSpace(af_Inherit *ih) {
    return ih->vs;
}

ObjAPIUint getAPICount(af_ObjectAPI *api) {
    return api->count;
}

void objectSetAllowInherit(af_Object *obj, bool allow) {
    obj->data->allow_inherit = allow;
}
