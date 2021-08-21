#ifndef AFUN__GC_H_PUBLIC
#define AFUN__GC_H_PUBLIC
#include "object.h"
#include "var.h"

/* gc 引用计数管理函数 */
#define gc_addReference(obj) ((obj)->gc.info.reference++)
#define gc_delReference(obj) ((obj)->gc.info.reference--)

/* gc 对象新增函数 */
void gc_addObject(af_Object *obj, af_Environment *env);
void gc_addVar(af_Var *obj, af_Environment *env);
void gc_addVarSpace(af_VarSpace *obj, af_Environment *env);

/* gc 运行时函数 */
bool resetGC(af_Environment *env);

#endif //AFUN__GC_H_PUBLIC
