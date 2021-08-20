#ifndef AFUN__GC_H_PUBLIC
#define AFUN__GC_H_PUBLIC
#include "object.h"
#include "var.h"

#define gc_addReference(obj) ((obj)->gc.info.reference++)
#define gc_delReference(obj) ((obj)->gc.info.reference--)

void gc_addObject(af_Object *obj, af_Environment *env);
void gc_addVar(af_Var *obj, af_Environment *env);
void gc_addVarSpace(af_VarSpace *obj, af_Environment *env);

bool resetGC(af_Environment *env);

#endif //AFUN__GC_H_PUBLIC
