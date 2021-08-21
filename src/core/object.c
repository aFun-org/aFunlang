﻿#include "__object.h"
#include "__env.h"
#include "tool.h"

/* ObjectData 创建与释放 */
static af_ObjectData *makeObjectData_Pri(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit);
static af_Object *makeObject_Pri(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit);

/* ObjectData API 创建与释放 */
static af_ObjectAPINode *makeObjectAPINode(DLC_SYMBOL(pAPIFUNC) func, char *api_name);
static af_ObjectAPINode *freeObjectAPINode(af_ObjectAPINode *apin);
static void freeAllObjectAPINode(af_ObjectAPINode *apin);

/* ObjectData API 管理函数 */
static af_ObjectAPINode *findObjectDataAPINode(char *api_name, af_ObjectData *od);
static int addAPIToObjectData(DLC_SYMBOL(pAPIFUNC) func, char *api_name, af_ObjectData *od);

static af_ObjectData *makeObjectData_Pri(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit) {
    af_ObjectData *od = calloc(sizeof(af_ObjectData), 1);
    od->id = strCopy(id == NULL ? "Unknow" : id);

    // data通过调用api实现
    od->size = 0;

    od->api = api;
    od->free_api = free_api;
    od->allow_inherit = allow_inherit;

    od->var_space = makeVarSpace();
    od->inherit = NULL;

    od->base = NULL;
    return od;
}

static af_Object *makeObject_Pri(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit) {
    af_Object *obj = calloc(sizeof(af_Object), 1);
    obj->belong = NULL;
    obj->data = makeObjectData_Pri(id, free_api, api, allow_inherit);
    obj->data->base = obj;
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
    if (env->core->in_init || inherit != NULL)
        ih = inherit;
    else if (env->core->object != NULL)
        ih = makeInherit(env->core->object);

    if (!env->core->in_init && ih == NULL)
        return NULL;

    af_Object *obj = makeObject_Pri(id, free_api, api, allow_inherit);

    if (env->core->in_init || belong != NULL)
        obj->belong = belong;
    else if (env->activity != NULL)
        obj->belong = env->activity->belong;
    else
        return NULL;

    obj->data->inherit = ih;
    gc_addObjectData(obj->data, env);
    gc_addObject(obj, env);
    return obj;
}

/*
 * 函数名: freeObjectData
 * 目标: 释放ObjectData, 仅GC和freeObject函数可用
 * 对外API中, 创建对象的基本单位都是af_Object, 无法直接操控af_ObjectData
 */
void freeObjectData(af_ObjectData *od) {
    free(od->id);
    free(od->data);
    if (od->free_api)
        freeObjectAPI(od->api);
    if (!od->var_space->gc.info.start_gc)
        freeVarSpace(od->var_space);
    freeAllInherit(od->inherit);
    GC_FREE_EXCHANGE(od);
    free(od);
}

void freeObject(af_Object *obj) {
    GC_FREE_EXCHANGE(obj);
    free(obj);
}

af_Object *getBelongObject(af_Object *object, af_Environment *env) {
    if (object->belong == NULL)
        return object;
    return object->belong;
}

af_Inherit *makeInherit(af_Object *obj) {
    af_Inherit *ih = calloc(sizeof(af_Inherit), 1);
    ih->obj = obj;  // 调用API获取vs
    return ih;
}

af_Inherit *freeInherit(af_Inherit *ih) {
    af_Inherit *next = ih->next;  // vs一定是被gc托管的
    free(ih);
    return next;
}

void freeAllInherit(af_Inherit *ih) {
    while (ih != NULL)
        ih = freeInherit(ih);
}

static af_ObjectAPINode *makeObjectAPINode(DLC_SYMBOL(pAPIFUNC) func, char *api_name) {
    if (func == NULL)
        return NULL;

    af_ObjectAPINode *apin = calloc(sizeof(af_ObjectAPINode), 1);
    apin->api = COPY_SYMBOL(func, pAPIFUNC);
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
    af_ObjectAPI *api = calloc(sizeof(af_ObjectAPI), 1);
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
int addAPI(DLC_SYMBOL(pAPIFUNC) func, char *api_name, af_ObjectAPI *api) {
    time33_t index = time33(api_name) % API_HASHTABLE_SIZE;
    af_ObjectAPINode **pNode = &api->node[index];

    for (NULL; *pNode != NULL; pNode = &((*pNode)->next)) {
        if (EQ_STR((*pNode)->name, api_name))
            return 0;
    }

    *pNode = makeObjectAPINode(func, api_name);
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
static int addAPIToObjectData(DLC_SYMBOL(pAPIFUNC) func, char *api_name,
                              af_ObjectData *od) {
    time33_t index = time33(api_name) % API_HASHTABLE_SIZE;
    af_ObjectAPINode **pNode = &od->api->node[index];

    for (NULL; *pNode != NULL; pNode = &((*pNode)->next)) {
        if (EQ_STR((*pNode)->name, api_name))
            return 0;
    }

    *pNode = makeObjectAPINode(func, api_name);
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
int addAPIToObject(DLC_SYMBOL(pAPIFUNC) func, char *api_name,
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
