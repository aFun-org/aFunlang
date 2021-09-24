#ifndef AFUN_OBJECT
#define AFUN_OBJECT
#include "aFunCoreExport.h"
#include "tool.h"

typedef uint32_t ObjAPIUint;
typedef struct af_Object af_Object;
typedef struct af_Inherit af_Inherit;
typedef struct af_ObjectAPI af_ObjectAPI;

/* 对象API函数 DLC */
typedef void objectAPIFunc();
DEFINE_DLC_SYMBOL(objectAPIFunc);

#include "obj_api.h"
#include "env.h"
#include "var.h"

/* 对象创建 */
AFUN_CORE_EXPORT af_Object *makeObject(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Object *belong,
                                       af_Inherit *inherit, af_Environment *env);

/* API表 创建与释放 */
AFUN_CORE_EXPORT af_ObjectAPI *makeObjectAPI(void);
AFUN_CORE_EXPORT void freeObjectAPI(af_ObjectAPI *api);

/* 对象继承表 创建与释放 */
AFUN_CORE_EXPORT af_Inherit *makeInherit(af_Object *obj);
AFUN_CORE_EXPORT void freeAllInherit(af_Inherit *ih);

/* 对象 相关操作 */
AFUN_CORE_EXPORT bool setObjectAttributes(char *name, char p_self, char p_posterity, char p_external, af_Object *attributes,
                                          af_Object *obj, af_Object *visitor, af_Environment *env);
AFUN_CORE_EXPORT void objectSetAllowInherit(af_Object *obj, bool allow);

/* API表 相关操作 */
AFUN_CORE_EXPORT int addAPI(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_ObjectAPI *api);
AFUN_CORE_EXPORT int addAPIToObject(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_Object *obj);

/* 对象继承表 相关操作 */
AFUN_CORE_EXPORT af_Inherit **pushInherit(af_Inherit **base, af_Inherit *new);

/* 对象 属性访问 */
AFUN_CORE_EXPORT void *getObjectData(af_Object *obj);
AFUN_CORE_EXPORT af_Object *getBelongObject(af_Object *object);
AFUN_CORE_EXPORT af_Object *findObjectAttributes(char *name, af_Object *visitor, af_Object *obj);
AFUN_CORE_EXPORT char *getObjectID(af_Object *obj);
AFUN_CORE_EXPORT af_ObjectAPI *getObjectAPI(af_Object *obj);
AFUN_CORE_EXPORT af_Inherit *getObjectInherit(af_Object *obj);

/* API表 属性访问 */
AFUN_CORE_EXPORT void *findAPI(char *api_name, af_ObjectAPI *api);
AFUN_CORE_EXPORT void *findObjectAPI(char *api_name, af_Object *obj);
AFUN_CORE_EXPORT ObjAPIUint getAPICount(af_ObjectAPI *api);

/* 对象继承表 属性访问 */
AFUN_CORE_EXPORT bool checkPosterity(af_Object *base, af_Object *posterity);
AFUN_CORE_EXPORT af_Inherit *getInheritNext(af_Inherit *ih);
AFUN_CORE_EXPORT af_Object *getInheritObject(af_Inherit *ih);
AFUN_CORE_EXPORT af_VarSpace *getInheritVarSpace(af_Inherit *ih);
#endif //AFUN_OBJECT
