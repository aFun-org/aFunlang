#ifndef AFUN__GC_H_PUBLIC
#define AFUN__GC_H_PUBLIC
#include "object.h"
#include "var.h"

/* gc 引用计数管理函数 */
#define gc_addReference(obj) ((_Generic((obj), \
                               af_Object *:gc_addObjectReference, \
                               af_Var *: gc_addVarReference, \
                               af_VarSpace *: gc_addVarSpaceReference))(obj))

#define gc_delReference(obj) ((_Generic((obj), \
                               af_Object *: gc_delObjectReference, \
                               af_Var *: gc_delVarReference, \
                               af_VarSpace *: gc_delVarSpaceReference))(obj))

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
bool resetGC(af_Environment *env);

#endif //AFUN__GC_H_PUBLIC
