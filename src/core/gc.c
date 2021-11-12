#include "aFunCore.h"
#include "__object.h"
#include "__var.h"
#include "__gc.h"
#include "__env.h"

/* gc 操控函数 */

void gc_addObjectData(af_ObjectData *obj, af_Environment *env){
    obj->gc.prev = ((void *) 0);
    if (env->gc_ObjectData != ((void *) 0))
        env->gc_ObjectData->gc.prev = obj;
    obj->gc.next = env->gc_ObjectData;
    env->gc_ObjectData = obj;
    GcCountAdd1(env);
}

void gc_addObjectDataReference(af_ObjectData *obj){
    obj->gc.info.reference++;
}

void gc_delObjectDataReference(af_ObjectData *obj){
    obj->gc.info.reference--;
}

GcCount gc_getObjectDataReference(af_ObjectData *obj){
    return obj->gc.info.reference;
}

void gc_addObject(af_Object *obj, af_Environment *env){
    obj->gc.prev = ((void *) 0);
    if (env->gc_Object != ((void *) 0))
        env->gc_Object->gc.prev = obj;
    obj->gc.next = env->gc_Object;
    env->gc_Object = obj;
    GcCountAdd1(env);
}

void gc_addObjectReference(af_Object *obj){
    obj->gc.info.reference++;
}

void gc_delObjectReference(af_Object *obj){
    obj->gc.info.reference--;
}

GcCount gc_getObjectReference(af_Object *obj){
    return obj->gc.info.reference;
}

void gc_addVar(af_Var *obj, af_Environment *env) {
    obj->gc.prev = ((void *) 0);
    if (env->gc_Var != ((void *) 0))
        env->gc_Var->gc.prev = obj;
    obj->gc.next = env->gc_Var;
    env->gc_Var = obj;
    GcCountAdd1(env);
}

void gc_addVarReference(af_Var *obj) {
    obj->gc.info.reference++;
}

void gc_delVarReference(af_Var *obj) {
    obj->gc.info.reference--;
}

GcCount gc_getVarReference(af_Var *obj) {
    return obj->gc.info.reference;
}

void gc_addVarSpace(af_VarSpace *obj, af_Environment *env){
    obj->gc.prev = ((void *) 0);
    if (env->gc_VarSpace != ((void *) 0)) { env->gc_VarSpace->gc.prev = obj; }
    obj->gc.next = env->gc_VarSpace;
    env->gc_VarSpace = obj;
    GcCountAdd1(env);
}

void gc_addVarSpaceReference(af_VarSpace *obj) {
    obj->gc.info.reference++;
}

void gc_delVarSpaceReference(af_VarSpace *obj) {
    obj->gc.info.reference--;
}

GcCount gc_getVarSpaceReference(af_VarSpace *obj) {
    return obj->gc.info.reference;
}

/* gcList 函数 */
/* gcList 创建与释放 */
static af_GcList *freeGcList(af_GcList *gl);

af_GcList *makeGcList(enum af_GcListType type, void *data) {
    af_GcList *gl = calloc(1, sizeof(af_GcList));
    gl->type = type;
    gl->data = data;
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
static pgc_Analyzed reachableObject(struct af_Object *od, pgc_Analyzed plist);

/* gc运行函数 */
static void freeValue(af_Environment *env);
static pgc_Analyzed reachable(af_Activity *activity, pgc_Analyzed plist);
static pgc_Analyzed iterLinker(af_Environment *core, pgc_Analyzed plist);
static pgc_Analyzed checkDestruct(af_Environment *env, paf_GuardianList *pdl, pgc_Analyzed plist);
static pgc_Analyzed checkAnalyzed(gc_Analyzed *analyzed, pgc_Analyzed plist);

// 使用 gc_Analyzed 目的是令可达性分析程序不需要使用递归
// Object->OvjectData->VarSpace->Var; 仅允许单项调用, 不发生递归
// 当VarSpace, Var和ObjectData需要调用Object时, 则使用gc_Analyzed, 创建需要调用的链
// ObjectData可能要调用API, 因此其需要调用的对象是不确定的, 但只有Object需要gc_Analyzed
// VarSpace和Var的调用是确定的, 他们不会往回调用除Object外的其他量
// 所以gc_Analyzed记录Object就足够了
static pgc_Analyzed reachableObject(struct af_Object *od, pgc_Analyzed plist) {
    for (NULL; od != NULL && !od->gc.info.reachable; od = od->belong) {
        od->gc.info.reachable = true;
        if (!od->data->gc.info.reachable)
            plist = reachableObjectData(od->data, plist);
    }
    return plist;
}

static pgc_Analyzed reachableObjectData(struct af_ObjectData *od, pgc_Analyzed plist) {  // 暂时不考虑API调用
    if (od->gc.info.reachable)
        return plist;

    od->gc.info.reachable = true;
    plist = reachableVarSpace(od->var_space, plist);

    if (!od->base->gc.info.reachable)
        plist = makeAnalyzed(od->base, plist);

    for (af_Inherit *ih = od->inherit; ih != NULL; ih = ih->next) {
        if (!ih->obj->gc.info.reachable)
            plist = makeAnalyzed(ih->obj, plist);
        if (!ih->vs->gc.info.reachable)
            plist = reachableVarSpace(ih->vs, plist);
    }

    obj_getGcList *func = findAPI("obj_getGcList", od->api);
    if (func != NULL) {
        af_GcList *gl = func(od->id, od->base, od->data);
        for (af_GcList *tmp = gl; tmp != NULL; tmp = tmp->next) {
            switch (tmp->type) {
                case glt_obj:
                    if (!tmp->obj->gc.info.reachable)
                        plist = makeAnalyzed(od->base, plist);
                    break;
                case glt_var:
                    plist = reachableVar(tmp->var, plist);
                    break;
                case glt_vs:
                    plist = reachableVarSpace(tmp->vs, plist);
                    break;
                case glt_vsl:
                    plist = reachableVarSpaceList(tmp->vsl, plist);
                    break;
                default:
                    break;
            }
        }
        freeAllGcList(gl);
    }
    return plist;
}

static pgc_Analyzed reachableVarSpace(struct af_VarSpace *vs, pgc_Analyzed plist) {
    if (vs->gc.info.reachable)
        return plist;

    vs->gc.info.reachable = true;
    if (vs->belong != NULL)
        plist = makeAnalyzed(vs->belong, plist);
    for (int i = 0; i < VAR_HASHTABLE_SIZE; i++) {
        for (af_VarCup *var = vs->var[i]; var != NULL; var = var->next)
            plist = reachableVar(var->var, plist);
    }

    return plist;
}

static pgc_Analyzed reachableVar(struct af_Var *var, pgc_Analyzed plist) {
    if (var->gc.info.reachable)
        return plist;

    var->gc.info.reachable = true;
    for (af_VarNode *vn = var->vn; vn != NULL; vn = vn->next) {
        if (!vn->obj->gc.info.reachable)
            plist = makeAnalyzed(vn->obj, plist);
    }

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

    for (af_ObjectData *od = env->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.info.reachable && (od->gc.info.reference > 0 || od->gc.info.not_clear))
            plist = reachableObjectData(od, plist);
    }

    for (af_Object *obj = env->gc_Object; obj != NULL; obj = obj->gc.next) {
        if (!obj->gc.info.reachable && (obj->gc.info.reference > 0 || obj->gc.info.not_clear))
            plist = reachableObject(obj, plist);
    }

    for (af_VarSpace *vs = env->gc_VarSpace; vs != NULL; vs = vs->gc.next) {
        if (!vs->gc.info.reachable && (vs->gc.info.reference > 0 || vs->gc.info.not_clear))
            plist = reachableVarSpace(vs, plist);
    }

    for (af_Var *var = env->gc_Var; var != NULL; var = var->gc.next) {
        if (!var->gc.info.reachable && (var->gc.info.reference > 0 || var->gc.info.not_clear))
            plist = reachableVar(var, plist);
    }
    return plist;
}

static pgc_Analyzed reachable(af_Activity *activity, pgc_Analyzed plist) {
    for (NULL; activity != NULL; activity = activity->prev) {
        if (activity->belong != NULL)
            plist = reachableObject(activity->belong, plist);

        plist = reachableVarSpaceList(activity->run_varlist, plist);

        if (activity->type == act_guardian)  // gc不执行接下来的检查
            continue;

        if (activity->func != NULL)
            plist = reachableObject(activity->func, plist);

        if (activity->return_obj != NULL)
            plist = reachableObject(activity->return_obj, plist);

        if (activity->parentheses_call != NULL)
            plist = reachableObject(activity->parentheses_call, plist);

        plist = reachableVarSpaceList(activity->func_varlist, plist);
        plist = reachableVarSpaceList(activity->macro_varlist, plist);
    }
    return plist;
}

static pgc_Analyzed checkAnalyzed(gc_Analyzed *analyzed, pgc_Analyzed plist) {
    for (gc_Analyzed *done = analyzed; done != NULL; done = done->next)
        plist = reachableObject(done->obj, plist);
    return plist;
}

void resetGC(af_Environment *env) {
    for (af_ObjectData *od = env->gc_ObjectData; od != NULL; od = od->gc.next)
        od->gc.info.reachable = false;

    for (af_Object *obj = env->gc_Object; obj != NULL; obj = obj->gc.next)
        obj->gc.info.reachable = false;

    for (af_VarSpace *vs = env->gc_VarSpace; vs != NULL; vs = vs->gc.next)
        vs->gc.info.reachable = false;

    for (af_Var *var = env->gc_Var; var != NULL; var = var->gc.next)
        var->gc.info.reachable = false;
}

static void freeValue(af_Environment *env) {
    for (af_ObjectData *od = env->gc_ObjectData, *next; od != NULL; od = next) {
        next = od->gc.next;
        if (!od->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "ObjectData: %p", od);
            freeObjectData(od, env);
        }
    }

    for (af_Object *obj = env->gc_Object, *next; obj != NULL; obj = next) {
        next = obj->gc.next;
        if (!obj->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "Object: %p", obj);
            freeObject(obj, env);
        }
    }

    for (af_VarSpace *vs = env->gc_VarSpace, *next; vs != NULL; vs = next) {
        next = vs->gc.next;
        if (!vs->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "VarSpace: %p", vs);
            freeVarSpace(vs, env);
        }
    }

    for (af_Var *var = env->gc_Var, *next; var != NULL; var = next) {
        next = var->gc.next;
        if (!var->gc.info.reachable) {
            writeTrackLog(aFunCoreLogger, "Var: %p", var);
            freeVar(var, env);
        }
    }
}

static pgc_Analyzed checkDestruct(af_Environment *env, paf_GuardianList *pgl, pgc_Analyzed plist) {
    for (af_ObjectData *od = env->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.info.reachable && !od->gc.done_destruct) {
            af_Object *func = findObjectAttributesByObjectData(mg_gc_destruct, NULL, od);
            if (func == NULL)
                continue;
            od->gc.done_destruct = true;
            printf("od->base = %p\n", od->base);
            *pgl = pushGuardianList(od->base, func, *pgl);
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
    resetGC(env);

    plist = iterLinker(env, plist);  // 临时量分析 (临时量都是通过reference标记的)
    plist = reachable(env->activity, plist);
    plist = checkAnalyzed(analyzed, plist);  // 先处理剩余的Object
    plist = checkDestruct(env, &pgl, plist);  // 在检查析构
    checkAnalyzed(analyzed, plist);  // 在处理 checkDestruct 时产生的新引用

    freeValue(env);
    freeAllAnalyzed(analyzed);
    return gl;
}

paf_GuardianList checkAllDestruct(af_Environment *env, paf_GuardianList pgl) {
    for (af_ObjectData *od = env->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.done_destruct) {
            af_Object *func = findObjectAttributesByObjectData(mg_gc_destruct, NULL, od);
            if (func == NULL)
                continue;
            od->gc.done_destruct = true;
            pgl = pushGuardianList(od->base, func, pgl);
        }
    }
    return pgl;
}

void gc_freeAllValueData(af_Environment *env) {
    for (af_ObjectData *od = env->gc_ObjectData; od != NULL; od = od->gc.next) {
        freeObjectDataData(od, env);
    }
}

void gc_freeAllValue(af_Environment *env) {
    for (af_ObjectData *od = env->gc_ObjectData, *next; od != NULL; od = next) {
        next = od->gc.next;
        freeObjectData(od, env);
    }

    for (af_Object *obj = env->gc_Object, *next; obj != NULL; obj = next) {
        next = obj->gc.next;
        freeObject(obj, env);
    }

    for (af_VarSpace *vs = env->gc_VarSpace, *next; vs != NULL; vs = next) {
        next = vs->gc.next;
        freeVarSpace(vs, env);
    }

    for (af_Var *var = env->gc_Var, *next; var != NULL; var = next) {
        next = var->gc.next;
        freeVar(var, env);
    }
}

void printGCByCore(af_Environment *env) {
    bool success = true;
    for (af_ObjectData *od = env->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (od->gc.info.reference != 0) {
            writeWarningLog(aFunCoreLogger, "af_ObjectData(%p) Reference: %d", od, od->gc.info.reference);
            success = false;
        } else
            writeTrackLog(aFunCoreLogger, "af_ObjectData(%p) Reference: %d", od, od->gc.info.reference);
    }

    for (af_Object *obj = env->gc_Object; obj != NULL; obj = obj->gc.next) {
        if (obj->gc.info.reference != 0) {
            writeWarningLog(aFunCoreLogger, "af_Object(%p->%p) Reference: %d", obj, obj->data, obj->gc.info.reference);
            success = false;
        } else
            writeTrackLog(aFunCoreLogger, "af_Object(%p->%p) Reference: %d", obj, obj->data, obj->gc.info.reference);
    }

    for (af_VarSpace *vs = env->gc_VarSpace; vs != NULL; vs = vs->gc.next) {
        if (vs->gc.info.reference != 0) {
            writeWarningLog(aFunCoreLogger, "af_VarSpace(%p) Reference: %d", vs, vs->gc.info.reference);
            success = false;
        } else
            writeTrackLog(aFunCoreLogger, "af_VarSpace(%p) Reference: %d", vs, vs->gc.info.reference);
    }

    for (af_Var *var = env->gc_Var; var != NULL; var = var->gc.next) {
        if (var->gc.info.reference != 0) {
            writeWarningLog(aFunCoreLogger, "af_Var(%p) Reference: %d", var, var->gc.info.reference);
            success = false;
        } else
            writeTrackLog(aFunCoreLogger, "af_Var(%p) Reference: %d", var, var->gc.info.reference);
    }

    if (!success)
        writeWarningLog(aFunCoreLogger, "gc warning.");
}
