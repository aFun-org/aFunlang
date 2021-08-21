/*
 * 文件名: __object.h
 * 目标: 定义aft对象的结构体
 */

#ifndef AFUN__OBJECT_H
#define AFUN__OBJECT_H
#include "macro.h"
#include "tool.h"

// 这些typedef可能会被下面include的文件使用
typedef struct af_ObjectData af_ObjectData;
typedef struct af_ObjectAPINode af_ObjectAPINode;
typedef struct af_ObjectAPI af_ObjectAPI;

#include "object.h"
#include "__gc.h"

#define API_HASHTABLE_SIZE (8)

struct af_ObjectAPINode {
    char *name;  // api名字
    DLC_SYMBOL(pAPIFUNC) api;  // api函数
    struct af_ObjectAPINode *next;
};

struct af_ObjectAPI {
    uint32_t count;  // api个数记录
    struct af_ObjectAPINode *(node[API_HASHTABLE_SIZE]);
};

struct af_ObjectData {
    char *id;  // 对象类型标识符(一个字符串)

    void *data;
    size_t size;  // 标记data的大小

    struct af_ObjectAPI *api;  // 继承的api必须位于Inherit链中
    bool allow_inherit;  // 是否允许被继承
    bool inherit_api;  // api被继承

    struct af_VarSpace *var_space;
    struct af_Inherit *iherit;  // 只有顶级继承对象的iherit属性可为NULL

    struct af_Object *base;  // 在gc机制中, 当对想要被释放前, 调用析构函数是对象以该base的身份出现
    GC_ObjectData gc;
};

struct af_Object {
    struct af_Object *belong;  // 只有顶级属对象的belong属性可为NULL
    struct af_ObjectData *data;
    GC_Object gc;
};

struct af_Inherit {
    struct af_Object *obj;
    struct af_Inherit *next;
};

void freeObjectData(af_ObjectData *od);  // gc使用
int addAPIToObjectData(DLC_SYMBOL(pAPIFUNC) func, char *api_name,
                       af_ObjectData *od);
af_ObjectAPINode *findObjectDataAPINode(char *api_name, af_ObjectData *od);

#endif //AFUN__OBJECT_H
