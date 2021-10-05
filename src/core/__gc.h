#ifndef AFUN_GC_H_
#define AFUN_GC_H_
#include "tool.h"

typedef struct GC_Var GC_Var;
typedef struct GC_VarSpace GC_VarSpace;
typedef struct GC_Object GC_Object;
typedef struct GC_ObjectData GC_ObjectData;
typedef struct af_GcList af_GcList;
typedef struct gc_Analyzed gc_Analyzed, **pgc_Analyzed;
typedef struct gc_DestructList gc_DestructList, **pgc_DestructList;

#define GC_FREE_EXCHANGE(obj, Type, Core) do { \
{if ((obj)->gc.prev != NULL) {(obj)->gc.prev->gc.next = (obj)->gc.next;} \
 else {(Core)->gc_##Type = (obj)->gc.next;}} \
{if ((obj)->gc.next != NULL) {(obj)->gc.next->gc.prev = (obj)->gc.prev;}}} while(0)

#define GC_CHAIN(type) struct type *next, *prev
typedef uint32_t GcCount;

struct gc_info {
    bool not_clear;  // 不清除
    GcCount reference;  // 引用计数
    bool reachable;  // 可达标记 [同时标识已迭代]
};

struct GC_ObjectData {
    struct gc_info info;
    GC_CHAIN(af_ObjectData);
    bool done_destruct;  // 是否已析构
};

struct GC_Object {
    struct gc_info info;
    GC_CHAIN(af_Object);
};

struct GC_Var {
    struct gc_info info;
    GC_CHAIN(af_Var);
};


struct GC_VarSpace {
    struct gc_info info;
    GC_CHAIN(af_VarSpace);
};

#undef GC_CHAIN

#include "__env.h"  // 这部分内容依赖上面的定义
#include "gc.h"

struct af_GcList {
    enum af_GcListType type;

    union {
        void *data;
        struct af_Object *obj;
        struct af_VarSpace *vs;
        struct af_Var *var;
        struct af_VarSpaceListNode *vsl;
    };

    struct af_GcList *next;
};

struct gc_Analyzed {
    struct af_Object *obj;
    struct gc_Analyzed *next;
};

struct gc_DestructList {
    struct af_Object *obj;
    struct af_Object *func;
    struct gc_DestructList *next;
};

/* 重新定义包括af_ObjectData的 gc Reference 函数 */
#ifdef core_shared_t_EXPORTS
#undef gc_addReference
#undef gc_delReference
#undef gc_getReference
#define gc_addReference(obj) ((_Generic((obj), \
                               af_ObjectData *: gc_addObjectDataReference, \
                               af_Object *: gc_addObjectReference, \
                               af_Var *: gc_addVarReference, \
                               af_VarSpace *: gc_addVarSpaceReference))(obj))

#define gc_delReference(obj) ((_Generic((obj), \
                               af_ObjectData *: gc_delObjectDataReference, \
                               af_Object *: gc_delObjectReference, \
                               af_Var *: gc_delVarReference, \
                               af_VarSpace *: gc_delVarSpaceReference))(obj))

#define gc_getReference(obj) ((_Generic((obj), \
                               af_ObjectData *: gc_getObjectDataReference, \
                               af_Object *: gc_getObjectReference, \
                               af_Var *: gc_getVarReference, \
                               af_VarSpace *: gc_getVarSpaceReference))(obj))
#endif
/* gc 对象新增函数 */
AFUN_CORE_NO_EXPORT void gc_addObject(af_Object *obj, af_Environment *env);
AFUN_CORE_NO_EXPORT void gc_addVar(af_Var *obj, af_Environment *env);
AFUN_CORE_NO_EXPORT void gc_addVarSpace(af_VarSpace *obj, af_Environment *env);
AFUN_CORE_NO_EXPORT void gc_addObjectData(struct af_ObjectData *obj, af_Environment *env);
AFUN_CORE_NO_EXPORT void gc_addObjectDataByCore(struct af_ObjectData *obj, af_Core *core);
AFUN_CORE_NO_EXPORT void gc_addObjectByCore(struct af_Object *obj, af_Core *core);
AFUN_CORE_NO_EXPORT void gc_addVarByCore(struct af_Var *obj, af_Core *core);
AFUN_CORE_NO_EXPORT void gc_addVarSpaceByCore(struct af_VarSpace *obj, af_Core *core);

/* gc Reference 管理函数 : 涉及af_ObjectData 不对外公开 */
AFUN_CORE_NO_EXPORT void gc_addObjectDataReference(af_ObjectData *obj);
AFUN_CORE_NO_EXPORT void gc_delObjectDataReference(af_ObjectData *obj);
AFUN_CORE_NO_EXPORT GcCount gc_getObjectDataReference(af_ObjectData *obj);

/* gc 操控函数 : gc的启动由解释器完全管理 */
AFUN_CORE_NO_EXPORT void gc_RunGC(af_Environment *env);
AFUN_CORE_NO_EXPORT pgc_DestructList checkAllDestruct(af_Environment *env, pgc_DestructList pdl);
AFUN_CORE_NO_EXPORT void gc_freeAllValueData(af_Environment *env);
AFUN_CORE_NO_EXPORT void gc_freeAllValue(af_Environment *env);

/* gc 信息函数 */
AFUN_CORE_NO_EXPORT void printGCByCore(af_Core *core);

/* gc_DestructList 释放函数*/
AFUN_CORE_NO_EXPORT void freeAllDestructList(gc_DestructList *dl);

/* gc 运行时函数 */
AFUN_CORE_NO_EXPORT void resetGC(af_Environment *env);

#endif //AFUN_GC_H_
