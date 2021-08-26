#include "aFun.h"
#include "__object.h"
#include "__var.h"
#include "__gc.h"
#include "__env.h"

/* gcList 函数 */
static af_GcList *makeGcList(enum af_GcListType type, void *data);
static af_GcList *freeGcList(af_GcList *gl);
static void freeAllGcList(af_GcList *gl);

static af_GcList *makeGcList(enum af_GcListType type, void *data) {
    af_GcList *gl = calloc(sizeof(af_GcList), 1);
    gl->type = type;
    gl->data = data;
    return gl;
}

static af_GcList *freeGcList(af_GcList *gl) {
    af_GcList *next = gl->next;
    free(gl);
    return next;
}

static void freeAllGcList(af_GcList *gl) {
    while (gl != NULL)
        gl = freeGcList(gl);
}

af_GcList *pushGcList(enum af_GcListType type, void *data, af_GcList *base) {
    af_GcList *next = makeGcList(type, data);
    next->next = base;
    return next;
}

/* gc 运行时函数 */
typedef struct gc_Analyzed gc_Analyzed, **pgc_Analyzed;
struct gc_Analyzed {
    enum gc_AnalyzedType {
        gc_ObjectData,
        gc_Object,
        gc_Var,
        gc_VarSpace,
    } type;

    union {
        void *data;  // 统一操控指针
        struct af_ObjectData *od;
        struct af_Object *obj;
        struct af_Var *var;
        struct af_VarSpace *vs;
    };

    struct gc_Analyzed *next;
};

/* 分析记录器创建与释放函数 */
static gc_Analyzed *makeAnalyzed(enum gc_AnalyzedType type, void *data);
static gc_Analyzed *freeAnalyzed(gc_Analyzed *base);

/* 指定都西昂的分析记录器创建与释放函数 : 调用分析记录器创建与释放函数 */
static pgc_Analyzed newObjectDataAnalyzed(struct af_ObjectData *od, pgc_Analyzed plist);
static pgc_Analyzed newObjectAnalyzed(struct af_Object *obj, pgc_Analyzed plist);
static pgc_Analyzed newVarAnalyzed(struct af_Var *var, pgc_Analyzed plist);
static pgc_Analyzed newVarSpaceAnalyzed(struct af_VarSpace *vs, pgc_Analyzed plist);

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
static void freeAllAnalyzed(gc_Analyzed *base);

static gc_Analyzed *makeAnalyzed(enum gc_AnalyzedType type, void *data) {
    gc_Analyzed *analyzed = calloc(sizeof(gc_Analyzed), 1);
    analyzed->type = type;
    analyzed->data = data;
    return analyzed;
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

static pgc_Analyzed newObjectDataAnalyzed(struct af_ObjectData *od, pgc_Analyzed plist) {
    if (od->gc.info.reachable)
        return plist;

    *plist = makeAnalyzed(gc_ObjectData, od);
    return &((*plist)->next);
}

static pgc_Analyzed newObjectAnalyzed(struct af_Object *obj, pgc_Analyzed plist) {
    if (obj->gc.info.reachable)
        return plist;

    *plist = makeAnalyzed(gc_Object, obj);
    return &((*plist)->next);
}

static pgc_Analyzed newVarAnalyzed(struct af_Var *var, pgc_Analyzed plist) {
    if (var->gc.info.reachable)
        return plist;

    *plist = makeAnalyzed(gc_Var, var);
    return &((*plist)->next);
}

static pgc_Analyzed newVarSpaceAnalyzed(struct af_VarSpace *vs, pgc_Analyzed plist) {
    if (vs->gc.info.reachable)
        return plist;

    *plist = makeAnalyzed(gc_VarSpace, vs);
    return &((*plist)->next);
}

// 使用 gc_Analyzed 目的是令可达性分析程序不需要使用递归
// Object->OvjectData->VarSpace->Var; 仅允许单项调用, 不发生递归
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
        plist = newObjectAnalyzed(od->base, plist);

    for (af_Inherit *ih = od->inherit; ih != NULL; ih = ih->next) {
        if (!ih->obj->gc.info.reachable)
            plist = newObjectAnalyzed(ih->obj, plist);
        if (!ih->vs->gc.info.reachable)
            plist = reachableVarSpace(ih->vs, plist);
    }

    obj_getGcList *func = findAPI("obj_getGcList", od->api);
    if (func != NULL) {
        af_GcList *gl = func(od->id, od->data);
        for (af_GcList *tmp = gl; tmp != NULL; tmp = tmp->next) {
            switch (tmp->type) {
                case glt_obj:
                    if (!tmp->obj->gc.info.reachable)
                        plist = newObjectAnalyzed(od->base, plist);
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
            plist = newObjectAnalyzed(vn->obj, plist);
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
    if (core->object != NULL)
        plist = reachableObject(core->object, plist);
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

        if (activity->func != NULL)
            plist = reachableObject(activity->func, plist);

        if (activity->return_obj != NULL)
            plist = reachableObject(activity->return_obj, plist);

        if (activity->parentheses_call != NULL)
            plist = reachableObject(activity->parentheses_call, plist);

        plist = reachableVarSpaceList(activity->var_list, plist);
        plist = reachableVarSpaceList(activity->func_var_list, plist);
        plist = reachableVarSpaceList(activity->macro_vsl, plist);
    }
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
        if (!od->gc.info.reachable) { // 暂时不考虑析构函数
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

#define GC_FUNC_DEFINED(type) \
void gc_add##type(af_##type *obj, af_Environment *env) { \
    obj->gc.prev = NULL; \
    if (env->core->gc_##type != NULL) { \
        env->core->gc_##type->gc.prev = obj; \
    }                             \
    obj->gc.next = env->core->gc_##type; \
    env->core->gc_##type = obj; \
} \
void gc_add##type##ByCore(af_##type *obj, af_Core *core) { \
if (obj->gc.next != NULL || obj->gc.prev != NULL) {return;} \
obj->gc.prev = NULL; \
obj->gc.next = core->gc_##type; \
core->gc_##type = obj; \
} \
void gc_add##type##Reference(af_##type *obj) { \
    obj->gc.info.reference++; \
} \
void gc_del##type##Reference(af_##type *obj) { \
    obj->gc.info.reference--; \
}

GC_FUNC_DEFINED(ObjectData)
GC_FUNC_DEFINED(Object)
GC_FUNC_DEFINED(Var)
GC_FUNC_DEFINED(VarSpace)

#undef GC_FUNC_DEFINED

void gc_RunGC(af_Environment *env) {
    gc_Analyzed *analyzed = NULL;
    pgc_Analyzed plist = &analyzed;
    resetGC(env);

    plist = iterLinker(env->core, plist);  // 临时量分析 (临时量都是通过reference标记的)
    plist = reachable(env->activity, plist);

    for (gc_Analyzed *done = analyzed; done != NULL; done = done->next) {
        switch (done->type) {
            case gc_ObjectData:
                plist = reachableObjectData(done->od, plist);
                break;
            case gc_Object:
                plist = reachableObject(done->obj, plist);
                break;
            case gc_VarSpace:
                plist = reachableVarSpace(done->vs, plist);
                break;
            case gc_Var:
                plist = reachableVar(done->var, plist);
                break;
            default:
                break;
        }
    }

    freeValue(env);
    freeAllAnalyzed(analyzed);
}

void gc_freeAllValue(af_Environment *env) {
    for (af_ObjectData *od = env->core->gc_ObjectData, *next; od != NULL; od = next) {
        next = od->gc.next;
        freeObjectData(od, env);  // 暂时不考虑析构函数
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
    printf("GC ObjectData:\n");
    for (af_ObjectData *od = core->gc_ObjectData; od != NULL; od = od->gc.next)
        printf("af_ObjectData(%p) Reference: %d\n", od, od->gc.info.reference);

    printf("GC Object:\n");
    for (af_Object *obj = core->gc_Object; obj != NULL; obj = obj->gc.next)
        printf("af_Object(%p->%p) Reference: %d\n", obj, obj->data, obj->gc.info.reference);

    printf("GC VarSpace:\n");
    for (af_VarSpace *vs = core->gc_VarSpace; vs != NULL; vs = vs->gc.next)
        printf("af_VarSpace(%p) Reference: %d\n", vs, vs->gc.info.reference);

    printf("GC Var:\n");
    for (af_Var *var = core->gc_Var; var != NULL; var = var->gc.next)
        printf("af_Var(%p) Reference: %d\n", var, var->gc.info.reference);

}
