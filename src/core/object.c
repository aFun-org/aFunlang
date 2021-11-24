#include "__object.h"
#include "__env.h"
#include "__gc.h"
#include "tool.h"
#include "core_init.h"


/* ObjectData API 创建与释放 */
static af_ObjectAPINode *makeObjectAPINode(DLC_SYMBOL(objectAPIFunc) func, char *api_name);
static af_ObjectAPINode *freeObjectAPINode(af_ObjectAPINode *apin);
static void freeAllObjectAPINode(af_ObjectAPINode *apin);

/* ObjectData API 管理函数 */
static af_ObjectAPINode *findObjectDataAPINode(char *api_name, af_ObjectData *od);
static int addAPIToObjectData(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_ObjectData *od);


/*
 * 函数名: 创建一个object
 * 目标: 生成Object和ObjectData, 并且添加到gc链表中
 * 若处于初始化模式, 则belong, inherit等可以设置为NULL, 由后期统一填上
 */
af_Object *makeObject(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Object *belong,
                      bool free_inherit, af_Inherit *inherit, af_Environment *env){
    enum af_CoreStatus status = getCoreStatus(env);

    if (api == NULL) {
        api = makeObjectAPI();
        free_api = true;
    }

    af_Inherit *ih = NULL;
    if (inherit != NULL)
        ih = inherit;
    else {
        free_inherit = true;
        if (env->global != NULL)
            ih = makeInherit(env->global);
        else if (status != core_creat)
            writeFatalErrorLog(aFunCoreLogger, 1, "Make object inherit null");
    }

    if (belong == NULL) {
        if (env->activity != NULL)
            belong = env->activity->belong;
        else if (status == core_init)  // init模式生成: global
            belong = env->global;
        else if (status != core_creat)  // 只有creat可以使用belong=NULL
            writeFatalErrorLog(aFunCoreLogger, 1, "Make object belong null");
    }

    af_Object *obj = calloc(1, sizeof(af_Object));
    af_ObjectData *od = calloc(1, sizeof(af_ObjectData));

    obj->belong = NULL;
    od->base = obj;
    obj->data = od;
    od->id = strCopy(id == NULL ? "Unknown" : id);

    od->api = api;
    od->free_api = free_api;

    od->allow_inherit = allow_inherit;
    od->inherit = NULL;

    obj_getDataSize *func = findAPI("obj_getDataSize", api);
    if (func != NULL)
        od->size = func(id, obj);
    else
        od->size = 0;

    if (od->size != 0)
        od->data = calloc(1, od->size);

    pthread_rwlock_init(&od->lock, NULL);
    pthread_rwlock_init(&obj->lock, NULL);

    obj->belong = belong;
    obj->data->inherit = ih;
    obj->data->free_inherit = free_inherit;

    od->var_space = makeVarSpace(obj, 3, 2, 0, env);
    gc_addObjectData(od, env->base);
    gc_addObject(obj, env->base);

    if (obj->data->size != 0) {  // 要在 add_object 之后再运行 init
        obj_initData *init = findAPI("obj_initData", obj->data->api);
        if (init != NULL)
            init(id, obj, obj->data->data, env);
    }

    gc_delReference(od->var_space, env);
    gc_delReference(od, env);
    return obj;
}

/*
 * 函数名: freeObjectDataData
 * 目标: 释放ObjectData的void *data, 仅GC函数可用
 * 对外API中, 创建对象的基本单位都是af_Object, 无法直接操控af_ObjectData
 */
void freeObjectDataData(af_ObjectData *od, af_Environment *env) {
    pthread_rwlock_rdlock(&od->lock);
    if (od->size == 0) {
        pthread_rwlock_unlock(&od->lock);
        return;
    }
    obj_destructData *func = findAPI("obj_destructData", od->api);
    if (func != NULL)
        func(od->id, od->base, od->data, env);
    od->size = 0;
    free(od->data);
    pthread_rwlock_unlock(&od->lock);
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
    if (od->free_inherit)
        freeAllInherit(od->inherit);
    gc_delObjectData(od, env);
    pthread_rwlock_destroy(&od->lock);
    free(od);
}

void freeObject(af_Object *obj, af_Environment *env) {
    gc_delObject(obj, env);
    pthread_rwlock_destroy(&obj->lock);
    free(obj);
}

void *getObjectData(af_Object *obj) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *od = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    pthread_rwlock_rdlock(&od->lock);
    void *data = od->data;
    pthread_rwlock_unlock(&od->lock);

    return data;
}

af_Object *getBelongObject(af_Object *object){
    pthread_rwlock_wrlock(&object->lock);
    af_Object *belong = (object->belong == NULL ? object : object->belong);
    pthread_rwlock_unlock(&object->lock);
    return belong;
}

af_Inherit *makeInherit(af_Object *obj) {
    if (!isObjectAllowInherit(obj))
        return NULL;

    obj_getShareVarSpace *func = findAPI("obj_getShareVarSpace", getObjectAPI(obj));
    af_VarSpace *vs = NULL;
    if (func == NULL)
        return NULL;

    if ((vs = func(getObjectID(obj), obj)) == NULL)
        return NULL;

    af_Inherit *ih = calloc(1, sizeof(af_Inherit));
    ih->vs = vs;
    ih->obj = obj;  // 调用API获取vs
    pthread_rwlock_init(&ih->lock, NULL);
    return ih;
}

/**
 * 压入Inherit到末尾
 * 注意: 不上锁
 * @param base
 * @param new
 * @return
 */
af_Inherit **pushInherit(af_Inherit **base, af_Inherit *new) {
    while ((*base) != NULL)
        base = &((*base)->next);
    *base = new;
    while ((*base) != NULL)
        base = &((*base)->next);
    return base;
}

static af_Inherit *freeInherit(af_Inherit *ih) {
    af_Inherit *next = ih->next;  // vs一定是被gc托管的
    pthread_rwlock_destroy(&ih->lock);
    free(ih);
    return next;
}

void freeAllInherit(af_Inherit *ih) {
    while (ih != NULL)
        ih = freeInherit(ih);
}

bool checkPosterity(af_Object *base, af_Object *posterity) {
    pthread_rwlock_rdlock(&posterity->lock);
    af_ObjectData *data = posterity->data;
    pthread_rwlock_unlock(&posterity->lock);

    for (af_Inherit *ih = getObjectInherit(base); ih != NULL; ih = getInheritNext(ih)) {
        af_Object *obj = getInheritObject(ih);
        pthread_rwlock_rdlock(&obj->lock);
        if (obj->data == data) {
            pthread_rwlock_unlock(&obj->lock);
            return true;
        }
        pthread_rwlock_unlock(&obj->lock);
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
    pthread_rwlock_init(&api->lock, NULL);
    return api;
}

void freeObjectAPI(af_ObjectAPI *api) {
    for (int i = 0; i < API_HASHTABLE_SIZE; i++)
        freeAllObjectAPINode(api->node[i]);
    pthread_rwlock_destroy(&api->lock);
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

    pthread_rwlock_wrlock(&api->lock);
    af_ObjectAPINode **pNode = &api->node[index];

    for (NULL; *pNode != NULL; pNode = &((*pNode)->next)) {
        if (EQ_STR((*pNode)->name, api_name)) {
            pthread_rwlock_unlock(&api->lock);
            return 0;
        }
    }

    *pNode = makeObjectAPINode(func, api_name);

    int res = -1;
    if (*pNode != NULL) {
        api->count++;
        res = 1;
    }

    pthread_rwlock_unlock(&api->lock);
    return res;
}

void *findAPI(char *api_name, af_ObjectAPI *api) {
    time33_t index = time33(api_name) % API_HASHTABLE_SIZE;
    void *data = NULL;
    pthread_rwlock_rdlock(&api->lock);

    for (af_ObjectAPINode *node = api->node[index]; node != NULL; node = node->next) {
        if (EQ_STR(node->name, api_name)) {
            data = GET_SYMBOL(node->api);
            break;
        }
    }

    pthread_rwlock_unlock(&api->lock);
    return data;
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

    pthread_rwlock_rdlock(&od->lock);
    af_ObjectAPI *api = od->api;
    pthread_rwlock_unlock(&od->lock);

    pthread_rwlock_wrlock(&api->lock);
    af_ObjectAPINode **pNode = &api->node[index];
    for (NULL; *pNode != NULL; pNode = &((*pNode)->next)) {
        if (EQ_STR((*pNode)->name, api_name)) {
            pthread_rwlock_unlock(&api->lock);
            return 0;
        }
    }

    *pNode = makeObjectAPINode(func, api_name);

    int res = -1;
    if (*pNode != NULL) {
        api->count++;
        res = 1;
    }

    pthread_rwlock_unlock(&api->lock);
    return res;
}

static af_ObjectAPINode *findObjectDataAPINode(char *api_name, af_ObjectData *od) {
    time33_t index = time33(api_name) % API_HASHTABLE_SIZE;

    pthread_rwlock_rdlock(&od->lock);
    af_ObjectAPI *api = od->api;
    pthread_rwlock_unlock(&od->lock);

    void *data = NULL;
    pthread_rwlock_rdlock(&api->lock);

    for (af_ObjectAPINode *node = api->node[index]; node != NULL; node = node->next) {
        if (EQ_STR(node->name, api_name)) {
            data = node;
            break;
        }
    }

    pthread_rwlock_unlock(&api->lock);
    return data;
}

/*
 * 函数名: findObjectDataAPINode
 * 目标: 从DLC中获取函数并写入Object的API
 */
int addAPIToObject(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_Object *obj) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *data = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    return addAPIToObjectData(func, api_name, data);
}

/*
 * 函数名: findObjectAPI
 * 目标: 从Object中获取指定api的函数指针
 */
void *findObjectAPI(char *api_name, af_Object *obj) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *data = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    af_ObjectAPINode *node = findObjectDataAPINode(api_name, data);
    if (node == NULL)
        return NULL;
    return GET_SYMBOL(node->api);
}

af_Object *findObjectAttributes(char *name, af_Object *visitor, af_Object *obj, af_Environment *env){
    af_Var *var = findVarFromVarSpace(name, visitor, getObjectVarSpace(obj));

    if (var != NULL)
        return findVarNode(var, NULL, env);

    for (af_Inherit *ih = getObjectInherit(obj); ih != NULL; ih = getInheritNext(ih)) {
        var = findVarFromVarSpace(name, visitor, getInheritVarSpace(ih));  // 搜索共享变量空间
        if (var != NULL)
            return findVarNode(var, NULL, env);
    }

    return NULL;
}

/**
 * 添加属性到Object中
 * 注意: 必须保证 obj 又被 gc 引用
 */
bool setObjectAttributes(char *name, char p_self, char p_posterity, char p_external, af_Object *attributes,
                         af_Object *obj, af_Object *visitor, af_Environment *env){
    return makeVarToVarSpace(name, p_self, p_posterity, p_external, attributes, getObjectVarSpace(obj), visitor, env);
}

/**
 * 获得指定对象的属性, 自动添加 gc_addReference
 * @param name
 * @param visitor
 * @param od
 * @return
 */
af_Object *findObjectAttributesByObjectData(char *name, af_Object *visitor, af_ObjectData *od, af_Environment *env){
    pthread_rwlock_rdlock(&od->lock);
    af_Var *var = findVarFromVarSpace(name, visitor, od->var_space);
    af_Inherit *ih = od->inherit;
    pthread_rwlock_unlock(&od->lock);

    if (var != NULL)
        return findVarNode(var, NULL, env);

    for (NULL; ih != NULL; ih = getInheritNext(ih)) {
        var = findVarFromVarSpace(name, visitor, getInheritVarSpace(ih));  // 搜索共享变量空间
        if (var != NULL)
            return findVarNode(var, NULL, env);
    }

    return NULL;
}

char *getObjectID(af_Object *obj) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *data = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    pthread_rwlock_rdlock(&data->lock);
    char *id = data->id;
    pthread_rwlock_unlock(&data->lock);

    return id;
}

af_ObjectAPI *getObjectAPI(af_Object *obj) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *data = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    pthread_rwlock_rdlock(&data->lock);
    af_ObjectAPI *api = data->api;
    pthread_rwlock_unlock(&data->lock);

    return api;
}

af_Inherit *getObjectInherit(af_Object *obj) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *data = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    pthread_rwlock_rdlock(&data->lock);
    af_Inherit *ih = data->inherit;
    pthread_rwlock_unlock(&data->lock);

    return ih;
}

af_VarSpace *getObjectVarSpace(af_Object *obj) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *data = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    pthread_rwlock_rdlock(&data->lock);
    af_VarSpace *vs = data->var_space;
    pthread_rwlock_unlock(&data->lock);

    return vs;
}

af_Inherit *getInheritNext(af_Inherit *ih) {
    pthread_rwlock_rdlock(&ih->lock);
    af_Inherit *next = ih->next;
    pthread_rwlock_unlock(&ih->lock);
    return next;
}

af_Object *getInheritObject(af_Inherit *ih) {
    pthread_rwlock_rdlock(&ih->lock);
    af_Object *obj = ih->obj;
    pthread_rwlock_unlock(&ih->lock);
    return obj;
}

af_VarSpace *getInheritVarSpace(af_Inherit *ih) {
    pthread_rwlock_rdlock(&ih->lock);
    af_VarSpace *vs = ih->vs;
    pthread_rwlock_unlock(&ih->lock);
    return vs;
}

ObjAPIUint getAPICount(af_ObjectAPI *api) {
    pthread_rwlock_rdlock(&api->lock);
    ObjAPIUint res = api->count;
    pthread_rwlock_unlock(&api->lock);
    return res;
}

void objectSetAllowInherit(af_Object *obj, bool allow) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *data = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    pthread_rwlock_rdlock(&data->lock);
    data->allow_inherit = allow;
    pthread_rwlock_unlock(&data->lock);
}

bool isObjectAllowInherit(af_Object *obj) {
    pthread_rwlock_rdlock(&obj->lock);
    af_ObjectData *data = obj->data;
    pthread_rwlock_unlock(&obj->lock);

    pthread_rwlock_rdlock(&data->lock);
    bool res = data->allow_inherit;
    pthread_rwlock_unlock(&data->lock);

    return res;
}
