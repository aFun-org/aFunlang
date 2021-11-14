﻿/*
 * 文件名: __object.h
 * 目标: 定义aft对象的结构体
 */

#ifndef AFUN_OBJECT_H_
#define AFUN_OBJECT_H_
#include "tool.h"
#include "obj_api.h"  // 该文件不包含在object.h中, object.h并不依赖该文件

// 这些typedef可能会被下面include的文件使用
typedef struct af_ObjectData af_ObjectData;
typedef struct af_ObjectAPINode af_ObjectAPINode;

#include "object.h"
#include "__gc.h"
#include "pthread.h"

#define API_HASHTABLE_SIZE (8)

typedef void objectAPIFunc();
NEW_DLC_SYMBOL(objectAPIFunc, objectAPIFunc);

struct af_ObjectAPINode {
    char *name;  // api名字
    DLC_SYMBOL(objectAPIFunc) api;  // api函数
    struct af_ObjectAPINode *next;
};

struct af_ObjectAPI {
    ObjAPIUint count;  // api个数记录
    struct af_ObjectAPINode *(node[API_HASHTABLE_SIZE]);
    pthread_rwlock_t lock;
};

struct af_ObjectData {
    char *id;  // 对象类型标识符(一个字符串)

    void *data;
    size_t size;  // 标记data的大小

    struct af_ObjectAPI *api;  // 继承的api必须位于Inherit链中
    bool allow_inherit;  // 是否允许被继承
    bool free_api;  // 是否释放api

    struct af_Inherit *inherit;  // 只有顶级继承对象的inherit属性可为NULL
    bool free_inherit;  // 是否释放inherit

    struct af_VarSpace *var_space;

    struct af_Object *base;  // 在gc机制中, 当对想要被释放前, 调用析构函数是对象以该base的身份出现
    pthread_rwlock_t lock;

    GC_ObjectData gc;
};

struct af_Object {
    struct af_Object *belong;  // 只有顶级属对象的belong属性可为NULL
    struct af_ObjectData *data;
    pthread_rwlock_t lock;  // 保护上面两个字段

    GC_Object gc;
};

struct af_Inherit {
    struct af_Object *obj;
    struct af_VarSpace *vs;  // 共享变量空间
    struct af_Inherit *next;
};

/* 对象 创建与释放 */
AFUN_CORE_NO_EXPORT void freeObject(af_Object *obj, af_Environment *env);

/* ObjectData 属性访问 */
AFUN_CORE_NO_EXPORT af_Object *findObjectAttributesByObjectData(char *name, af_Object *visitor, af_ObjectData *od);

/* ObjectData 释放函数 */
AFUN_CORE_NO_EXPORT void freeObjectDataData(af_ObjectData *od, af_Environment *env);
AFUN_CORE_NO_EXPORT void freeObjectData(af_ObjectData *od, af_Environment *env);

#endif //AFUN_OBJECT_H_
