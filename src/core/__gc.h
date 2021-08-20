#ifndef AFUN_GC_H
#define AFUN_GC_H
#include "macro.h"

typedef struct GC_Var GC_Var;
typedef struct GC_VarSpace GC_VarSpace;
typedef struct GC_Object GC_Object;
typedef struct GC_ObjectData GC_ObjectData;

#define GC_FREE_EXCHANGE(obj) do { \
if ((obj)->gc.prev != NULL) (obj)->gc.prev->gc.next = (obj)->gc.next; \
if ((obj)->gc.next != NULL) (obj)->gc.next->gc.prev = (obj)->gc.prev; } while(0)

#define GC_CHAIN(type) struct type *next, *prev
typedef uint32_t GcCount;

struct gc_info {
    bool start_gc;  // 启用gc
    bool not_clear;  // 不清除
    GcCount reference;  // 引用计数
    bool reachable;  // 可达标记 [同时标识已迭代]
};

struct GC_ObjectData {
    struct gc_info info;
    GC_CHAIN(af_ObjectData);
    bool done_del;  // 是否已析构
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

void gc_addObjectData(struct af_ObjectData *obj, af_Environment *env);  // af_ObjectData 不对外公开
void gc_addObjectDataByCore(struct af_ObjectData *obj, af_Core *core);
void gc_addObjectByCore(struct af_Object *obj, af_Core *core);
void gc_addVarByCore(struct af_Var *obj, af_Core *core);
void gc_addVarSpaceByCore(struct af_VarSpace *obj, af_Core *core);

bool gc_RunGC(af_Environment *env);
void gc_freeAllValue(af_Core *core);

#endif //AFUN_GC_H
