#ifndef AFUN_GC
#define AFUN_GC
#include "aFunCoreExport.h"
#include "object.h"
#include "var.h"

typedef uint32_t GcCount;
typedef struct af_GcList af_GcList;
enum af_GcListType {
    glt_obj = 1,
    glt_vs,
    glt_var,
    glt_vsl,
};

/* gc 引用计数管理函数 */
#define gc_addReference(obj) ((_Generic((obj), \
                               af_Object *:gc_addObjectReference, \
                               af_Var *: gc_addVarReference, \
                               af_VarSpace *: gc_addVarSpaceReference))(obj))

#define gc_delReference(obj) ((_Generic((obj), \
                               af_Object *: gc_delObjectReference, \
                               af_Var *: gc_delVarReference, \
                               af_VarSpace *: gc_delVarSpaceReference))(obj))

#define gc_getReference(obj) ((_Generic((obj), \
                               af_Object *: gc_getObjectReference, \
                               af_Var *: gc_getVarReference, \
                               af_VarSpace *: gc_getVarSpaceReference))(obj))

/* GcList 创建与释放 */
AFUN_CORE_EXPORT af_GcList *makeGcList(enum af_GcListType type, void *data);
AFUN_CORE_EXPORT void freeAllGcList(af_GcList *gl);

/* GcList 相关操作 */
AFUN_CORE_EXPORT af_GcList *pushGcList(enum af_GcListType type, void *data, af_GcList *base);

/* gc Reference 相关操作 */
AFUN_CORE_EXPORT void gc_addObjectReference(af_Object *obj);
AFUN_CORE_EXPORT void gc_addVarReference(af_Var *obj);
AFUN_CORE_EXPORT void gc_addVarSpaceReference(af_VarSpace *obj);
AFUN_CORE_EXPORT void gc_delObjectReference(af_Object *obj);
AFUN_CORE_EXPORT void gc_delVarReference(af_Var *obj);
AFUN_CORE_EXPORT void gc_delVarSpaceReference(af_VarSpace *obj);

/* gc Reference 属性访问 */
AFUN_CORE_EXPORT GcCount gc_getObjectReference(af_Object *obj);
AFUN_CORE_EXPORT GcCount gc_getVarReference(af_Var *obj);
AFUN_CORE_EXPORT GcCount gc_getVarSpaceReference(af_VarSpace *obj);
#endif //AFUN_GC
