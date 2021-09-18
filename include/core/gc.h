#ifndef AFUN_GC
#define AFUN_GC
#include "object.h"
#include "var.h"

typedef struct af_GcList af_GcList;
typedef struct gc_DestructList gc_DestructList;
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

/* gc_DestructList 释放函数*/
void freeAllDestructList(gc_DestructList *dl);

/* GcList 操作函数 */
af_GcList *pushGcList(enum af_GcListType type, void *data, af_GcList *base);

/* gc 对象新增函数 */
void gc_addObject(af_Object *obj, af_Environment *env);
void gc_addVar(af_Var *obj, af_Environment *env);
void gc_addVarSpace(af_VarSpace *obj, af_Environment *env);

/* gc Reference 管理函数 */
void gc_addObjectReference(af_Object *obj);
void gc_addVarReference(af_Var *obj);
void gc_addVarSpaceReference(af_VarSpace *obj);
void gc_delObjectReference(af_Object *obj);
void gc_delVarReference(af_Var *obj);
void gc_delVarSpaceReference(af_VarSpace *obj);

/* gc 运行时函数 */
void resetGC(af_Environment *env);

#endif //AFUN_GC
