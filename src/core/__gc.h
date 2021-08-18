#ifndef AFUN_GC_H
#define AFUN_GC_H
#include "macro.h"
#include "env.h"
#include "gc.h"

#define GC_CHAIN(type) struct type *next, *prev;
typedef uint32_t GcCount;

struct gc_info {
    bool start_gc;  // 启用gc
    bool not_clear;  // 不清除
    GcCount *reference;  // 引用计数
    bool reachable;  // 可达标记 [同时标识已迭代]
};

typedef struct GC_ObjectData GC_ObjectData;
struct GC_ObjectData {
    struct gc_info info;
    GC_CHAIN(af_ObjectData);
    bool done_del;  // 是否已析构
};

typedef struct GC_Object GC_Object;
struct GC_Object {
    struct gc_info info;
    GC_CHAIN(af_Object);
};

typedef struct GC_Var GC_Var;
struct GC_Var {
    struct gc_info info;
    GC_CHAIN(af_Var);
};

typedef struct GC_VarSpace GC_VarSpace;
struct GC_VarSpace {
    struct gc_info info;
    GC_CHAIN(af_VarSpace);
};

#undef GC_CHAIN

bool gc_RunGC(af_Environment *env);

#endif //AFUN_GC_H
