#ifndef AFUN_GC_H_
#define AFUN_GC_H_
#include "tool.h"
#include "pthread.h"

typedef struct GC_Var GC_Var;
typedef struct GC_VarSpace GC_VarSpace;
typedef struct GC_Object GC_Object;
typedef struct GC_ObjectData GC_ObjectData;
typedef struct af_GcList af_GcList;
typedef struct gc_Analyzed gc_Analyzed, **pgc_Analyzed;
typedef struct gc_Factory gc_Factory;

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

struct gc_Factory {
    pthread_mutex_t mutex;  // 互斥锁, 保护下面字段
    struct af_ObjectData *gc_ObjectData;
    struct af_Object *gc_Object;
    struct af_Var *gc_Var;
    struct af_VarSpace *gc_VarSpace;
};

/* 重新定义包括af_ObjectData的 gc Reference 函数 */
#undef gc_addReference
#undef gc_delReference
#define gc_addReference(obj, env) ((_Generic((obj), \
                               af_ObjectData *: gc_addObjectDataReference, \
                               af_Object *: gc_addObjectReference, \
                               af_Var *: gc_addVarReference, \
                               af_VarSpace *: gc_addVarSpaceReference))((obj), (env)))

#define gc_delReference(obj, env) ((_Generic((obj), \
                               af_ObjectData *: gc_delObjectDataReference, \
                               af_Object *: gc_delObjectReference, \
                               af_Var *: gc_delVarReference, \
                               af_VarSpace *: gc_delVarSpaceReference, \
                               af_VarSpaceListNode *: gc_delVarListReference))((obj), (env)))

/* gc_Factory 创建与释放 */
AFUN_CORE_NO_EXPORT gc_Factory *makegGcFactory(void);
AFUN_CORE_NO_EXPORT void freeGcFactory(gc_Factory *factory);

/* gc 对象新增函数 */
AFUN_CORE_NO_EXPORT void gc_addObject(af_Object *obj, af_Environment *base);
AFUN_CORE_NO_EXPORT void gc_addVar(af_Var *obj, af_Environment *base);
AFUN_CORE_NO_EXPORT void gc_addVarSpace(af_VarSpace *obj, af_Environment *base);
AFUN_CORE_NO_EXPORT void gc_addObjectData(struct af_ObjectData *obj, af_Environment *base);

AFUN_CORE_NO_EXPORT void gc_delObject(af_Object *obj, af_Environment *base);
AFUN_CORE_NO_EXPORT void gc_delVar(af_Var *obj, af_Environment *base);
AFUN_CORE_NO_EXPORT void gc_delVarSpace(af_VarSpace *obj, af_Environment *base);
AFUN_CORE_NO_EXPORT void gc_delObjectData(struct af_ObjectData *obj, af_Environment *base);

/* gc Reference 管理函数 : 涉及af_ObjectData 不对外公开 */
AFUN_CORE_NO_EXPORT void gc_addObjectDataReference(af_ObjectData *obj, af_Environment *base);
AFUN_CORE_NO_EXPORT void gc_delObjectDataReference(af_ObjectData *obj, af_Environment *base);

/* gc 操控函数 : gc的启动由解释器完全管理 */
AFUN_CORE_NO_EXPORT af_GuardianList *gc_RunGC(af_Environment *env);
AFUN_CORE_NO_EXPORT paf_GuardianList checkAllDestruct(af_Environment *env, paf_GuardianList pgl);
AFUN_CORE_NO_EXPORT void gc_freeAllValueData(af_Environment *env);
AFUN_CORE_NO_EXPORT void gc_freeAllValue(af_Environment *env);

/* gc 信息函数 */
AFUN_CORE_NO_EXPORT void printGCByCore(af_Environment *env);

/* gc 运行时函数 */
AFUN_CORE_NO_EXPORT void resetGC(af_Environment *env);

#endif //AFUN_GC_H_
