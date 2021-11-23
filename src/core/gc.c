#include "aFunCore.h"
#include "__object.h"
#include "__var.h"
#include "__gc.h"
#include "__env.h"
#include "pthread.h"

/* gc 操控函数 */
void gc_addObjectData(af_ObjectData *od, af_Environment *base){
    pthread_mutex_lock(&base->gc_factory->mutex);
    od->gc.info.reference = 1;
    od->gc.prev = ((void *) 0);
    if (base->gc_factory->gc_ObjectData != ((void *) 0))
        base->gc_factory->gc_ObjectData->gc.prev = od;
    od->gc.next = base->gc_factory->gc_ObjectData;
    base->gc_factory->gc_ObjectData = od;
    GcCountAdd1(base);
    pthread_mutex_unlock(&base->gc_factory->mutex);
    writeTrackLog(aFunCoreLogger, "Make ObjectData %p", od);
}

void gc_delObjectData(af_ObjectData *od, af_Environment *base){
    pthread_mutex_lock(&base->gc_factory->mutex);
    if ((od)->gc.prev != ((void *) 0)) { (od->gc.prev->gc.next = (od)->gc.next; }
    else { base->gc_factory->gc_ObjectData = (od)->gc.next; }
    if ((od)->gc.next != ((void *) 0)) { (od)->gc.next->gc.prev od)->gc.prev; }
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_addObjectDataReference(af_ObjectData *od, af_Environment *base){
    base = base->base;  // 转换为主线程 Env

    pthread_mutex_lock(&base->gc_factory->mutex);
    od->gc.info.reference++;
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_delObjectDataReference(af_ObjectData *od, af_Environment *base){
    base = base->base;  // 转换为主线程 Env

    pthread_mutex_lock(&base->gc_factory->mutex);
    od->gc.info.reference--;
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_addObject(af_Object *obj, af_Environment *base){
    pthread_mutex_lock(&base->gc_factory->mutex);
    obj->gc.info.reference = 1;
    obj->gc.prev = ((void *) 0);
    if (base->gc_factory->gc_Object != ((void *) 0))
        base->gc_factory->gc_Object->gc.prev = obj;
    obj->gc.next = base->gc_factory->gc_Object;
    base->gc_factory->gc_Object = obj;
    GcCountAdd1(base);
    pthread_mutex_unlock(&base->gc_factory->mutex);
    writeTrackLog(aFunCoreLogger, "Make Object %p", obj);
}

void gc_delObject(af_Object *obj, af_Environment *base){
    pthread_mutex_lock(&base->gc_factory->mutex);
    if ((obj)->gc.prev != ((void *) 0)) { (obj->gc.prev->gc.next = (obj)->gc.next; }
    else { base->gc_factory->gc_Object = (obj)->gc.next; }
    if ((obj)->gc.next != ((void *) 0)) { (obj)->gc.next->gc.prev = (obj)->gc.prev; }
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_addObjectReference(af_Object *obj, af_Environment *base){
    base = base->base;  // 转换为主线程 Env

    pthread_mutex_lock(&base->gc_factory->mutex);
    obj->gc.info.reference++;
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_delObjectReference(af_Object *obj, af_Environment *base){
    base = base->base;  // 转换为主线程 Env

    pthread_mutex_lock(&base->gc_factory->mutex);
    obj->gc.info.reference--;
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_addVar(af_Var *var, af_Environment *base) {
    var->gc.info.reference = 1;
    var->gc.prev = ((void *) 0);
    if (base->gc_factory->gc_Var != ((void *) 0))
        base->gc_factory->gc_Var->gc.prev = var;
    var->gc.next = base->gc_factory->gc_Var;
    base->gc_factory->gc_Var = var;
    GcCountAdd1(base);
    writeTrackLog(aFunCoreLogger, "Make Var %p", var);
}

void gc_delVar(af_Var *var, af_Environment *base){
    pthread_mutex_lock(&base->gc_factory->mutex);
    if ((var)->gc.prev != ((void *) 0)) { (var)->gc.prev->gc.next = (var)->gc.next; }
    else { base->gc_factory->gc_Var = (var)->gc.next; }
    if ((var)->gc.next != ((void *) 0)) { (var)->gc.next->gc.prev = (var)->gc.prev; }
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_addVarReference(af_Var *var, af_Environment *base) {
    base = base->base;  // 转换为主线程 Env

    pthread_mutex_lock(&base->gc_factory->mutex);
    var->gc.info.reference++;
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_delVarReference(af_Var *var, af_Environment *base) {
    base = base->base;  // 转换为主线程 Env

    pthread_mutex_lock(&base->gc_factory->mutex);
    var->gc.info.reference--;
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

/**
 * 将 VarSpace 添加到 gc 链中
 * 只能由MakeVarSpace调用
 * 注意: 不上锁
 * @param vs
 * @param base
 */
void gc_addVarSpace(af_VarSpace *vs, af_Environment *base){
    vs->gc.info.reference = 1;
    vs->gc.prev = ((void *) 0);
    if (base->gc_factory->gc_VarSpace != ((void *) 0)) { base->gc_factory->gc_VarSpace->gc.prev = vs; }
    vs->gc.next = base->gc_factory->gc_VarSpace;
    base->gc_factory->gc_VarSpace = vs;
    GcCountAdd1(base);
    writeTrackLog(aFunCoreLogger, "Make VarSpace %p", vs);
}

void gc_delVarSpace(af_VarSpace *vs, af_Environment *base){
    pthread_mutex_lock(&base->gc_factory->mutex);
    if ((vs)->gc.prev != ((void *) 0)) { (vs)->gc.prev->gc.next = (vs)->gc.next; }
    else { base->gc_factory->gc_VarSpace = (vs)->gc.next; }
    if ((vs)->gc.next != ((void *) 0)) { (vs)->gc.next->gc.prev = (vs)->gc.prev; }
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_addVarSpaceReference(af_VarSpace *vs, af_Environment *base) {
    base = base->base;  // 转换为主线程 Env

    pthread_mutex_lock(&base->gc_factory->mutex);
    vs->gc.info.reference++;
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

void gc_delVarSpaceReference(af_VarSpace *vs, af_Environment *base) {
    base = base->base;  // 转换为主线程 Env

    pthread_mutex_lock(&base->gc_factory->mutex);
    vs->gc.info.reference--;
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

/* gc_Factory 函数 */
gc_Factory *makegGcFactory(void) {
    gc_Factory *factory = calloc(1, sizeof(gc_Factory));

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&factory->mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    return factory;
}

void freeGcFactory(struct gc_Factory *factory) {
    pthread_mutex_destroy(&factory->mutex);
    free(factory);
}

/* gcList 函数 */
/* gcList 创建与释放 */
static af_GcList *freeGcList(af_GcList *gl);

af_GcList *makeGcList(enum af_GcListType type, void *data) {
    af_GcList *gl = calloc(1, sizeof(af_GcList));
    gl->type = type;
    gl->data = data;
    assertWarningLog(data != NULL, aFunCoreLogger, "GcList not data write.");
    return gl;
}

static af_GcList *freeGcList(af_GcList *gl) {
    af_GcList *next = gl->next;
    free(gl);
    return next;
}

void freeAllGcList(af_GcList *gl) {
    while (gl != NULL)
        gl = freeGcList(gl);
}

af_GcList *pushGcList(enum af_GcListType type, void *data, af_GcList *base) {
    af_GcList *next = makeGcList(type, data);
    next->next = base;
    return next;
}

/* 分析记录器函数 */
/* 分析记录器 创建与释放 */
static pgc_Analyzed makeAnalyzed(struct af_Object *obj, pgc_Analyzed plist);
static gc_Analyzed *freeAnalyzed(gc_Analyzed *base);
static void freeAllAnalyzed(gc_Analyzed *base);

// 关于gc_Analyzed为什么只需要记录Object的解释参见下文 (reachableObject)
static pgc_Analyzed makeAnalyzed(struct af_Object *obj, pgc_Analyzed plist) {
    if (obj->gc.info.reachable)
        return plist;

    *plist = calloc(1, sizeof(gc_Analyzed));
    (*plist)->obj = obj;
    return &((*plist)->next);
}

static gc_Analyzed *freeAnalyzed(gc_Analyzed *base) {
    gc_Analyzed *next = base->next;
    free(base);
    return next;
}

static void freeAllAnalyzed(gc_Analyzed *base) {
    while (base != NULL)
        base = freeAnalyzed(base);
}

/* gc 运行时函数 */
/* 可达性分析函数 */
static pgc_Analyzed reachableVar(struct af_Var *var, pgc_Analyzed plist);
static pgc_Analyzed reachableVarSpace(struct af_VarSpace *vs, pgc_Analyzed plist);
static pgc_Analyzed reachableVarSpaceList(struct af_VarSpaceListNode *vsl, pgc_Analyzed plist);
static pgc_Analyzed reachableObjectData(struct af_ObjectData *od, pgc_Analyzed plist);
static pgc_Analyzed reachableObject(struct af_Object *obj, pgc_Analyzed plist);

/* gc运行函数 */
static void freeValue(af_Environment *env);
static pgc_Analyzed reachable(af_Activity *activity, pgc_Analyzed plist);
static pgc_Analyzed iterLinker(af_Environment *env, pgc_Analyzed plist);
static pgc_Analyzed iterEnvironment(af_Environment *env, pgc_Analyzed plist);
static pgc_Analyzed checkDestruct(af_Environment *env, paf_GuardianList *pgl, pgc_Analyzed plist);
static pgc_Analyzed checkAnalyzed(gc_Analyzed *analyzed, pgc_Analyzed plist);

// 使用 gc_Analyzed 目的是令可达性分析程序不需要使用递归
// Object->OvjectData->VarSpace->Var; 仅允许单项调用, 不发生递归
// 当VarSpace, Var和ObjectData需要调用Object时, 则使用gc_Analyzed, 创建需要调用的链
// ObjectData可能要调用API, 因此其需要调用的对象是不确定的, 但只有Object需要gc_Analyzed
// VarSpace和Var的调用是确定的, 他们不会往回调用除Object外的其他量
// 所以gc_Analyzed记录Object就足够了
static pgc_Analyzed reachableObject(struct af_Object *obj, pgc_Analyzed plist) {
    for (NULL; obj != NULL && !obj->gc.info.reachable; obj = obj->belong) {
        obj->gc.info.reachable = true;
        writeTrackLog(aFunCoreLogger, "Reachable Object %p -> data %p -> vs %p", obj, obj->data, obj->data->var_space);

        pthread_rwlock_rdlock(&obj->lock);
        if (!obj->data->gc.info.reachable)
            plist = reachableObjectData(obj->data, plist);
        pthread_rwlock_unlock(&obj->lock);
    }
    return plist;
}

static pgc_Analyzed reachableObjectData(struct af_ObjectData *od, pgc_Analyzed plist) {  // 暂时不考虑API调用
    writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p wait", od);
    if (od->gc.info.reachable)
        return plist;

    writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p -> vs %p", od, od->var_space);
    od->gc.info.reachable = true;

    pthread_rwlock_rdlock(&od->lock);
    plist = reachableVarSpace(od->var_space, plist);
    if (!od->base->gc.info.reachable) {
        plist = makeAnalyzed(od->base, plist);
        writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p add object %p", od, od->base);
    }

    for (af_Inherit *ih = od->inherit; ih != NULL; ih = getInheritNext(ih)) {
        af_Object *obj = getInheritObject(ih);
        af_VarSpace *vs = getInheritVarSpace(ih);
        if (!obj->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p inherit add object %p", od, obj);
            plist = makeAnalyzed(obj, plist);
        }

        if (!vs->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p inherit add varspace %p", od, vs);
            plist = reachableVarSpace(vs, plist);
        }
    }

    obj_getGcList *func = findAPI("obj_getGcList", od->api);
    if (func != NULL) {
        af_GcList *gl = func(od->id, od->base, od->data);
        for (af_GcList *tmp = gl; tmp != NULL; tmp = tmp->next) {
            if (tmp->data == NULL)
                continue;

            switch (tmp->type) {
                case glt_obj:
                    if (!tmp->obj->gc.info.reachable) {
                        writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p gl add object %p", od, tmp->obj);
                        plist = makeAnalyzed(tmp->obj, plist);
                    }
                    break;
                case glt_var:
                    writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p gl add object %p", od, tmp->var);
                    plist = reachableVar(tmp->var, plist);
                    break;
                case glt_vs:
                    writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p gl add object %p", od, tmp->vs);
                    plist = reachableVarSpace(tmp->vs, plist);
                    break;
                case glt_vsl:
                    writeTrackLog(aFunCoreLogger, "Reachable ObjectData %p gl add object %p", od, tmp->vsl);
                    plist = reachableVarSpaceList(tmp->vsl, plist);
                    break;
                default:
                    break;
            }
        }
        freeAllGcList(gl);
    }

    pthread_rwlock_unlock(&od->lock);
    return plist;
}

static pgc_Analyzed reachableVarSpace(struct af_VarSpace *vs, pgc_Analyzed plist) {
    writeTrackLog(aFunCoreLogger, "Reachable VarSpace %p wait", vs);
    if (vs->gc.info.reachable)
        return plist;

    writeTrackLog(aFunCoreLogger, "Reachable VarSpace %p -> %p", vs, vs->belong);
    vs->gc.info.reachable = true;

    pthread_rwlock_rdlock(&vs->lock);
    if (vs->belong != NULL) {
        plist = makeAnalyzed(vs->belong, plist);
        writeTrackLog(aFunCoreLogger, "Reachable VarSpace %p add object %p", vs, vs->belong);
    }

    for (int i = 0; i < VAR_HASHTABLE_SIZE; i++) {
        for (af_VarCup *var = vs->var[i]; var != NULL; var = var->next)
            plist = reachableVar(var->var, plist);
    }

    pthread_rwlock_unlock(&vs->lock);
    return plist;
}

static pgc_Analyzed reachableVar(struct af_Var *var, pgc_Analyzed plist) {
    writeTrackLog(aFunCoreLogger, "Reachable Var %p wait", var);
    if (var->gc.info.reachable)
        return plist;

    writeTrackLog(aFunCoreLogger, "Reachable VarSpace %p -> %p [%s]", var, var->vn->obj, var->name);
    var->gc.info.reachable = true;

    pthread_rwlock_rdlock(&var->lock);
    for (af_VarNode *vn = var->vn; vn != NULL; vn = vn->next) {
        if (!vn->obj->gc.info.reachable) {
            plist = makeAnalyzed(vn->obj, plist);
            writeTrackLog(aFunCoreLogger, "Reachable Var %p add object %p", var, vn->obj);
        }
    }
    pthread_rwlock_unlock(&var->lock);
    return plist;
}

static pgc_Analyzed reachableVarSpaceList(struct af_VarSpaceListNode *vsl, pgc_Analyzed plist) {
    for (NULL; vsl != NULL; vsl = vsl->next) {
        if (!vsl->vs->gc.info.reachable)
            plist = reachableVarSpace(vsl->vs, plist);
    }
    return plist;
}

static pgc_Analyzed iterLinker(af_Environment *env, pgc_Analyzed plist) {
    plist = reachableVarSpace(env->protect, plist);
    if (env->global != NULL)
        plist = reachableObject(env->global, plist);

    pthread_mutex_lock(&env->gc_factory->mutex);
    for (af_ObjectData *od = env->gc_factory->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.info.reachable && (od->gc.info.reference > 0 || od->gc.info.not_clear))
            plist = reachableObjectData(od, plist);
    }

    for (af_Object *obj = env->gc_factory->gc_Object; obj != NULL; obj = obj->gc.next) {
        if (!obj->gc.info.reachable && (obj->gc.info.reference > 0 || obj->gc.info.not_clear))
            plist = reachableObject(obj, plist);
    }

    for (af_VarSpace *vs = env->gc_factory->gc_VarSpace; vs != NULL; vs = vs->gc.next) {
        if (!vs->gc.info.reachable && (vs->gc.info.reference > 0 || vs->gc.info.not_clear))
            plist = reachableVarSpace(vs, plist);
    }

    for (af_Var *var = env->gc_factory->gc_Var; var != NULL; var = var->gc.next) {
        if (!var->gc.info.reachable && (var->gc.info.reference > 0 || var->gc.info.not_clear))
            plist = reachableVar(var, plist);
    }
    pthread_mutex_unlock(&env->gc_factory->mutex);
    return plist;
}

static pgc_Analyzed iterEnvironment(af_Environment *env, pgc_Analyzed plist) {
    pthread_mutex_lock(&env->thread_lock);
    if (env->result != NULL)
        plist = reachableObject(env->result, plist);
    pthread_mutex_unlock(&env->thread_lock);

    plist = reachable(env->activity, plist);
    return plist;
}

static pgc_Analyzed reachable(af_Activity *activity, pgc_Analyzed plist) {
    for (NULL; activity != NULL; activity = activity->prev) {
        pthread_rwlock_rdlock(&activity->gc_lock);
        if (activity->belong != NULL)
            plist = reachableObject(activity->belong, plist);

        plist = reachableVarSpaceList(activity->run_varlist, plist);

        if (activity->type == act_guardian) {  // gc不执行接下来的检查
            pthread_rwlock_unlock(&activity->gc_lock);
            continue;
        }

        if (activity->func != NULL)
            plist = reachableObject(activity->func, plist);

        if (activity->return_obj != NULL)
            plist = reachableObject(activity->return_obj, plist);

        if (activity->parentheses_call != NULL)
            plist = reachableObject(activity->parentheses_call, plist);

        plist = reachableVarSpaceList(activity->out_varlist, plist);
        plist = reachableVarSpaceList(activity->func_varlist, plist);
        plist = reachableVarSpaceList(activity->macro_varlist, plist);
        pthread_rwlock_unlock(&activity->gc_lock);
    }
    return plist;
}

static pgc_Analyzed checkAnalyzed(gc_Analyzed *analyzed, pgc_Analyzed plist) {
    for (gc_Analyzed *done = analyzed; done != NULL; done = done->next)
        plist = reachableObject(done->obj, plist);
    return plist;
}

void resetGC(af_Environment *env) {
    pthread_mutex_lock(&env->gc_factory->mutex);
    for (af_ObjectData *od = env->gc_factory->gc_ObjectData; od != NULL; od = od->gc.next)
        od->gc.info.reachable = false;

    for (af_Object *obj = env->gc_factory->gc_Object; obj != NULL; obj = obj->gc.next)
        obj->gc.info.reachable = false;

    for (af_VarSpace *vs = env->gc_factory->gc_VarSpace; vs != NULL; vs = vs->gc.next)
        vs->gc.info.reachable = false;

    for (af_Var *var = env->gc_factory->gc_Var; var != NULL; var = var->gc.next)
        var->gc.info.reachable = false;
    pthread_mutex_unlock(&env->gc_factory->mutex);
}

/**
 * 清理对象, 清除不可达的对象
 * 内部函数, 不上锁
 * @param env
 */
static void freeValue(af_Environment *env) {
    for (af_ObjectData *od = env->gc_factory->gc_ObjectData, *next; od != NULL; od = next) {
        next = od->gc.next;
        if (!od->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "GC free ObjectData: %p", od);
            freeObjectData(od, env);
        }
    }

    for (af_Object *obj = env->gc_factory->gc_Object, *next; obj != NULL; obj = next) {
        next = obj->gc.next;
        if (!obj->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "GC free Object: %p", obj);
            freeObject(obj, env);
        }
    }

    for (af_VarSpace *vs = env->gc_factory->gc_VarSpace, *next; vs != NULL; vs = next) {
        next = vs->gc.next;
        if (!vs->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "GC free VarSpace: %p [%p]", vs, vs->belong);
            freeVarSpace(vs, env);
        }
    }

    for (af_Var *var = env->gc_factory->gc_Var, *next; var != NULL; var = next) {
        next = var->gc.next;
        if (!var->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "GC free Var: %p [%s]", var, var->name);
            freeVar(var, env);
        }
    }
}

/**
 * 检查对象是否有析构函数
 * 内部函数, 不上锁
 * @param env
 * @param pgl
 * @param plist
 * @return
 */
static pgc_Analyzed checkDestruct(af_Environment *env, paf_GuardianList *pgl, pgc_Analyzed plist) {
    for (af_ObjectData *od = env->gc_factory->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.info.reachable && !od->gc.done_destruct) {
            af_Object *func = findObjectAttributesByObjectData(mg_gc_destruct, NULL, od, env);
            if (func == NULL)
                continue;
            od->gc.done_destruct = true;

            pthread_rwlock_rdlock(&od->lock);
            af_Object *base = od->base;
            pthread_rwlock_unlock(&od->lock);
            gc_addReference(base, env);

            *pgl = pushGuardianList(base, func, *pgl, env);
            plist = reachableObjectData(od, plist);
        }
    }
    return plist;
}

af_GuardianList *gc_RunGC(af_Environment *env) {
    gc_Analyzed *analyzed = NULL;
    af_GuardianList *gl = NULL;
    pgc_Analyzed plist = &analyzed;
    paf_GuardianList pgl = &gl;
    af_Environment *base = env->base;

    pthread_mutex_lock(&base->gc_factory->mutex);
    resetGC(base);
    plist = iterLinker(base, plist);  // 临时量分析 (临时量都是通过reference标记的)
    plist = iterEnvironment(base, plist);

    pthread_mutex_lock(&base->thread_lock);
    for (af_EnvironmentList *envl = base->env_list; envl != NULL; envl = envl->next)
        plist = iterEnvironment(envl->env, plist);
    pthread_mutex_unlock(&base->thread_lock);

    plist = checkAnalyzed(analyzed, plist);  // 先处理剩余的Object
    plist = checkDestruct(base, &pgl, plist);  // 在检查析构
    checkAnalyzed(analyzed, plist);  // 在处理 checkDestruct 时产生的新引用

    pthread_mutex_unlock(&base->gc_factory->mutex);
    freeValue(base);
    freeAllAnalyzed(analyzed);
    return gl;
}

/**
 * 检查所有ObjectData的析构函数
 * 非内部函数, 会上锁
 * @param env
 * @param pgl
 * @return
 */
paf_GuardianList checkAllDestruct(af_Environment *env, paf_GuardianList pgl) {
    af_Environment *base = env->base;

    pthread_mutex_lock(&base->gc_factory->mutex);
    for (af_ObjectData *od = base->gc_factory->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.done_destruct) {
            af_Object *func = findObjectAttributesByObjectData(mg_gc_destruct, NULL, od, env);
            if (func == NULL)
                continue;
            od->gc.done_destruct = true;

            pthread_rwlock_rdlock(&od->lock);
            af_Object *base_obj = od->base;
            pthread_rwlock_unlock(&od->lock);
            gc_addReference(base_obj, env);
            pgl = pushGuardianList(base_obj, func, pgl, env);
        }
    }
    pthread_mutex_unlock(&base->gc_factory->mutex);
    return pgl;
}

/**
 * 释放所有ObjectData的 void *Data
 * @param env
 */
void gc_freeAllValueData(af_Environment *env) {
    af_Environment *base = env->base;

    pthread_mutex_lock(&base->gc_factory->mutex);
    for (af_ObjectData *od = base->gc_factory->gc_ObjectData; od != NULL; od = od->gc.next) {
        freeObjectDataData(od, env);
    }
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

/**
 * 释放所有的Value
 * @param env
 */
void gc_freeAllValue(af_Environment *env) {
    af_Environment *base = env->base;

    pthread_mutex_lock(&base->gc_factory->mutex);
    for (af_ObjectData *od = base->gc_factory->gc_ObjectData, *next; od != NULL; od = next) {
        next = od->gc.next;
        freeObjectData(od, env);
    }

    for (af_Object *obj = base->gc_factory->gc_Object, *next; obj != NULL; obj = next) {
        next = obj->gc.next;
        freeObject(obj, env);
    }

    for (af_VarSpace *vs = base->gc_factory->gc_VarSpace, *next; vs != NULL; vs = next) {
        next = vs->gc.next;
        freeVarSpace(vs, env);
    }

    for (af_Var *var = base->gc_factory->gc_Var, *next; var != NULL; var = next) {
        next = var->gc.next;
        freeVar(var, env);
    }
    pthread_mutex_unlock(&base->gc_factory->mutex);
}

/**
 * Debug用的函数
 * 不上锁
 * @param env
 */
void printGCByCore(af_Environment *env) {
    bool success = true;
    for (af_ObjectData *od = env->gc_factory->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (od->gc.info.reference != 0) {
            writeWarningLog(aFunCoreLogger, "GC Reference ObjectData(%p): %d", od, od->gc.info.reference);
            success = false;
        } else
            writeTrackLog(aFunCoreLogger, "GC Reference ObjectData(%p): %d", od, od->gc.info.reference);
    }

    for (af_Object *obj = env->gc_factory->gc_Object; obj != NULL; obj = obj->gc.next) {
        if (obj->gc.info.reference != 0) {
            writeWarningLog(aFunCoreLogger, "GC Reference Object(%p->%p): %d", obj, obj->data, obj->gc.info.reference);
            success = false;
        } else
            writeTrackLog(aFunCoreLogger, "GC Reference Object(%p->%p): %d", obj, obj->data, obj->gc.info.reference);
    }

    for (af_VarSpace *vs = env->gc_factory->gc_VarSpace; vs != NULL; vs = vs->gc.next) {
        if (vs->gc.info.reference != 0) {
            writeWarningLog(aFunCoreLogger, "GC Reference VarSpace(%p): %d", vs, vs->gc.info.reference);
            success = false;
        } else
            writeTrackLog(aFunCoreLogger, "GC Reference VarSpace(%p): %d", vs, vs->gc.info.reference);
    }

    for (af_Var *var = env->gc_factory->gc_Var; var != NULL; var = var->gc.next) {
        if (var->gc.info.reference != 0) {
            writeWarningLog(aFunCoreLogger, "GC Reference Var(%p): %d", var, var->gc.info.reference);
            success = false;
        } else
            writeTrackLog(aFunCoreLogger, "GC Reference Var(%p): %d", var, var->gc.info.reference);
    }

    if (!success)
        writeWarningLog(aFunCoreLogger, "GC Reference warning.");
}
