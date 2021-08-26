#ifndef AFUN__OBJECT_H_PUBLIC
#define AFUN__OBJECT_H_PUBLIC
#include "tool.h"

typedef struct af_Object af_Object;
typedef struct af_Inherit af_Inherit;
typedef struct af_ObjectAPI af_ObjectAPI;

/* 对象API函数 DLC */
typedef void objectAPIFunc();
DEFINE_DLC_SYMBOL(objectAPIFunc);

#include "obj_api.h"
#include "env.h"

/* 对象创建与释放函数 */
af_Object *makeObject(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Object *belong,
                      af_Inherit *inherit, af_Environment *env);
void freeObject(af_Object *obj, af_Environment *env);

/* 对象属性获取函数 */
void *getObjectData(af_Object *obj);
af_Object *getBelongObject(af_Object *object, af_Environment *env);
af_Object *findObjectAttributes(char *name, af_Object *obj);

/* 对象继承关系函数 */
af_Inherit *makeInherit(af_Object *obj);
af_Inherit *freeInherit(af_Inherit *ih);
void freeAllInherit(af_Inherit *ih);

/* API表 创建与释放 */
af_ObjectAPI *makeObjectAPI(void);
void freeObjectAPI(af_ObjectAPI *api);

/* API表 管理函数 */
int addAPI(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_ObjectAPI *api);
void *findAPI(char *api_name, af_ObjectAPI *api);

/* 对象API函数 */
void *findObjectAPI(char *api_name, af_Object *obj);
int addAPIToObject(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_Object *obj);
#endif //AFUN__OBJECT_H_PUBLIC
