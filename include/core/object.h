#ifndef AFUN_OBJECT
#define AFUN_OBJECT
#include "aFunCoreExport.h"
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
AFUN_CORE_EXPORT af_Object *makeObject(char *id, bool free_api, af_ObjectAPI *api, bool allow_inherit, af_Object *belong,
                      af_Inherit *inherit, af_Environment *env);

/* 对象属性获取函数 */
AFUN_CORE_EXPORT void *getObjectData(af_Object *obj);
AFUN_CORE_EXPORT af_Object *getBelongObject(af_Object *object);
AFUN_CORE_EXPORT af_Object *findObjectAttributes(char *name, af_Object *visitor, af_Object *obj);

/* 对象属性设置 */
AFUN_CORE_EXPORT bool setObjectAttributes(char *name, char p_self, char p_posterity, char p_external, af_Object *attributes,
                         af_Object *obj, af_Object *visitor, af_Environment *env);

/* 对象继承关系函数 */
AFUN_CORE_EXPORT af_Inherit *makeInherit(af_Object *obj);
AFUN_CORE_EXPORT void freeAllInherit(af_Inherit *ih);

AFUN_CORE_EXPORT bool checkPosterity(af_Object *base, af_Object *posterity);

/* API表 创建与释放 */
AFUN_CORE_EXPORT af_ObjectAPI *makeObjectAPI(void);
AFUN_CORE_EXPORT void freeObjectAPI(af_ObjectAPI *api);

/* API表 管理函数 */
AFUN_CORE_EXPORT int addAPI(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_ObjectAPI *api);
AFUN_CORE_EXPORT void *findAPI(char *api_name, af_ObjectAPI *api);

/* 对象API函数 */
AFUN_CORE_EXPORT int addAPIToObject(DLC_SYMBOL(objectAPIFunc) func, char *api_name, af_Object *obj);
AFUN_CORE_EXPORT void *findObjectAPI(char *api_name, af_Object *obj);
#endif //AFUN_OBJECT
