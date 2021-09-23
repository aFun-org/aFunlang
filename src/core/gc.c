#include "aFunCore.h"
#include "__object.h"
#include "__var.h"
#include "__gc.h"
#include "__env.h"

/* gc 操控函数 */
#define GC_FUNC_DEFINED(type) \
void gc_add##type(af_##type *obj, af_Environment *env) { \
    obj->gc.prev = NULL; \
    if (env->core->gc_##type != NULL) { \
        env->core->gc_##type->gc.prev = obj; \
    }                             \
    obj->gc.next = env->core->gc_##type; \
    env->core->gc_##type = obj;  \
    env->core->gc_count++;  \
} \
void gc_add##type##ByCore(af_##type *obj, af_Core *core) { \
if (obj->gc.next != NULL || obj->gc.prev != NULL) {return;} \
obj->gc.prev = NULL; \
obj->gc.next = core->gc_##type; \
core->gc_##type = obj;        \
core->gc_count++;  \
} \
void gc_add##type##Reference(af_##type *obj) { \
    obj->gc.info.reference++; \
} \
void gc_del##type##Reference(af_##type *obj) { \
    obj->gc.info.reference--; \
} \
GcCount gc_get##type##Reference(af_##type *obj) { \
    return obj->gc.info.reference; \
}

GC_FUNC_DEFINED(ObjectData)
GC_FUNC_DEFINED(Object)
GC_FUNC_DEFINED(Var)
GC_FUNC_DEFINED(VarSpace)

#undef GC_FUNC_DEFINED

/* gc_DestructList 函数 */
/* gc_DestructList 创建与释放 */
static gc_DestructList *makeDestructList(af_ObjectData *od, af_Object *func);
static gc_DestructList *freeDestructList(gc_DestructList *dl);

/* gc_DestructList 相关操作 */
static pgc_DestructList pushDestructList(af_ObjectData *od, af_Object *func, pgc_DestructList pdl);

static gc_DestructList *makeDestructList(af_ObjectData *od, af_Object *func) {
    gc_DestructList *dl = calloc(1, sizeof(gc_DestructList));
    dl->obj = od->base;
    dl->func = func;
    gc_addReference(dl->obj);
    gc_addReference(dl->func);
    return dl;
}

static gc_DestructList *freeDestructList(gc_DestructList *dl) {
    gc_DestructList *next = dl->next;
    gc_delReference(dl->obj);
    gc_delReference(dl->func);
    free(dl);
    return next;
}

void freeAllDestructList(gc_DestructList *dl) {
    while (dl != NULL)
        dl = freeDestructList(dl);
}

static pgc_DestructList pushDestructList(af_ObjectData *od, af_Object *func, pgc_DestructList pdl) {
    *pdl = makeDestructList(od, func);
    return &((*pdl)->next);
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
static pgc_Analyzed iterLinker(af_Core *core, pgc_Analyzed plist);
static pgc_Analyzed checkDestruct(af_Environment *env, pgc_DestructList *pdl, pgc_Analyzed plist);
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

static pgc_Analyzed iterLinker(af_Core *core, pgc_Analyzed plist) {
    plist = reachableVarSpace(core->protect, plist);
    if (core->global != NULL)
        plist = reachableObject(core->global, plist);

    for (af_ObjectData *od = core->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.info.reachable && (od->gc.info.reference > 0 || od->gc.info.not_clear))
            plist = reachableObjectData(od, plist);
    }

    for (af_Object *obj = core->gc_Object; obj != NULL; obj = obj->gc.next) {
        if (!obj->gc.info.reachable && (obj->gc.info.reference > 0 || obj->gc.info.not_clear))
            plist = reachableObject(obj, plist);
    }

    for (af_VarSpace *vs = core->gc_VarSpace; vs != NULL; vs = vs->gc.next) {
        if (!vs->gc.info.reachable && (vs->gc.info.reference > 0 || vs->gc.info.not_clear))
            plist = reachableVarSpace(vs, plist);
    }

    for (af_Var *var = core->gc_Var; var != NULL; var = var->gc.next) {
        if (!var->gc.info.reachable && (var->gc.info.reference > 0 || var->gc.info.not_clear))
            plist = reachableVar(var, plist);
    }
    return plist;
}

static pgc_Analyzed reachable(af_Activity *activity, pgc_Analyzed plist) {
    for (NULL; activity != NULL; activity = activity->prev) {
        if (activity->belong != NULL)
            plist = reachableObject(activity->belong, plist);

        plist = reachableVarSpaceList(activity->var_list, plist);

        if (activity->type == act_gc)  // gc不执行接下来的检查
            continue;

        if (activity->func != NULL)
            plist = reachableObject(activity->func, plist);

        if (activity->return_obj != NULL)
            plist = reachableObject(activity->return_obj, plist);

        if (activity->parentheses_call != NULL)
            plist = reachableObject(activity->parentheses_call, plist);

        plist = reachableVarSpaceList(activity->func_var_list, plist);
        plist = reachableVarSpaceList(activity->macro_vsl, plist);
    }
    return plist;
}

static pgc_Analyzed checkAnalyzed(gc_Analyzed *analyzed, pgc_Analyzed plist) {
    for (gc_Analyzed *done = analyzed; done != NULL; done = done->next)
        plist = reachableObject(done->obj, plist);
    return plist;
}

void resetGC(af_Environment *env) {
    for (af_ObjectData *od = env->core->gc_ObjectData; od != NULL; od = od->gc.next)
        od->gc.info.reachable = false;

    for (af_Object *obj = env->core->gc_Object; obj != NULL; obj = obj->gc.next)
        obj->gc.info.reachable = false;

    for (af_VarSpace *vs = env->core->gc_VarSpace; vs != NULL; vs = vs->gc.next)
        vs->gc.info.reachable = false;

    for (af_Var *var = env->core->gc_Var; var != NULL; var = var->gc.next)
        var->gc.info.reachable = false;
}

static void freeValue(af_Environment *env) {
    for (af_ObjectData *od = env->core->gc_ObjectData, *next; od != NULL; od = next) {
        next = od->gc.next;
        if (!od->gc.info.reachable) {
            printf("- gc free ObjectData: %p\n", od);
            freeObjectData(od, env);
        }
    }

    for (af_Object *obj = env->core->gc_Object, *next; obj != NULL; obj = next) {
        next = obj->gc.next;
        if (!obj->gc.info.reachable) {
            printf("- gc free Object: %p\n", obj);
            freeObjectByCore(obj, env->core);
        }
    }

    for (af_VarSpace *vs = env->core->gc_VarSpace, *next; vs != NULL; vs = next) {
        next = vs->gc.next;
        if (!vs->gc.info.reachable) {
            printf("- gc free VarSpace: %p\n", vs);
            freeVarSpaceByCore(vs, env->core);
        }
    }

    for (af_Var *var = env->core->gc_Var, *next; var != NULL; var = next) {
        next = var->gc.next;
        if (!var->gc.info.reachable) {
            printf("- gc free Var: %p\n", var);
            freeVarByCore(var, env->core);
        }
    }
}

static pgc_Analyzed checkDestruct(af_Environment *env, pgc_DestructList *pdl, pgc_Analyzed plist) {
    for (af_ObjectData *od = env->core->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.info.reachable && !od->gc.done_destruct) {
            af_Object *func = findObjectAttributesByObjectData(mg_gc_destruct, NULL, od);
            if (func == NULL)
                continue;
            od->gc.done_destruct = true;
            *pdl = pushDestructList(od, func, *pdl);
            plist = reachableObjectData(od, plist);
        }
    }
    return plist;
}

void gc_RunGC(af_Environment *env) {
    gc_Analyzed *analyzed = NULL;
    gc_DestructList *dl = NULL;
    pgc_Analyzed plist = &analyzed;
    pgc_DestructList pdl = &dl;
    resetGC(env);

    plist = iterLinker(env->core, plist);  // 临时量分析 (临时量都是通过reference标记的)
    plist = reachable(env->activity, plist);
    plist = checkAnalyzed(analyzed, plist);  // 先处理剩余的Object
    plist = checkDestruct(env, &pdl, plist);  // 在检查析构
    checkAnalyzed(analyzed, plist);  // 在处理 checkDestruct 时产生的新引用

    freeValue(env);
    freeAllAnalyzed(analyzed);
    if (dl != NULL)
        pushGCActivity(dl, pdl, env);
}

pgc_DestructList checkAllDestruct(af_Environment *env, pgc_DestructList pdl) {
    for (af_ObjectData *od = env->core->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (!od->gc.done_destruct) {
            af_Object *func = findObjectAttributesByObjectData(mg_gc_destruct, NULL, od);
            if (func == NULL)
                continue;
            od->gc.done_destruct = true;
            pdl = pushDestructList(od, func, pdl);
        }
    }
    return pdl;
}

void gc_freeAllValue(af_Environment *env) {
    for (af_ObjectData *od = env->core->gc_ObjectData, *next; od != NULL; od = next) {
        next = od->gc.next;
        freeObjectData(od, env);
    }

    for (af_Object *obj = env->core->gc_Object, *next; obj != NULL; obj = next) {
        next = obj->gc.next;
        freeObjectByCore(obj, env->core);
    }

    for (af_VarSpace *vs = env->core->gc_VarSpace, *next; vs != NULL; vs = next) {
        next = vs->gc.next;
        freeVarSpaceByCore(vs, env->core);
    }

    for (af_Var *var = env->core->gc_Var, *next; var != NULL; var = next) {
        next = var->gc.next;
        freeVarByCore(var, env->core);
    }
}

void printGCByCode(af_Core *core) {
    bool success = true;
    printf("GC ObjectData:\n");
    for (af_ObjectData *od = core->gc_ObjectData; od != NULL; od = od->gc.next) {
        if (od->gc.info.reference != 0) {
            printf("########## ########## ");
            success = false;
        }
        printf("af_ObjectData(%p) Reference: %d\n", od, od->gc.info.reference);
    }

    printf("GC Object:\n");
    for (af_Object *obj = core->gc_Object; obj != NULL; obj = obj->gc.next) {
        if (obj->gc.info.reference != 0) {
            printf("########## ########## ");
            success = false;
        }
        printf("af_Object(%p->%p) Reference: %d\n", obj, obj->data, obj->gc.info.reference);
    }

    printf("GC VarSpace:\n");
    for (af_VarSpace *vs = core->gc_VarSpace; vs != NULL; vs = vs->gc.next) {
        if (vs->gc.info.reference != 0) {
            printf("########## ########## ");
            success = false;
        }
        printf("af_VarSpace(%p) Reference: %d\n", vs, vs->gc.info.reference);
    }

    printf("GC Var:\n");
    for (af_Var *var = core->gc_Var; var != NULL; var = var->gc.next) {
        if (var->gc.info.reference != 0) {
            printf("########## ########## ");
            success = false;
        }
        printf("af_Var(%p) Reference: %d\n", var, var->gc.info.reference);
    }

    if (!success)
        printf("gc warning.\n");
}
