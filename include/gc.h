#ifndef AFUN__GC_H_PUBLIC
#define AFUN__GC_H_PUBLIC
#include "object.h"
#include "var.h"

void gc_addObjectData(af_ObjectData *obj, af_Core *core);
void gc_addObject(af_Object *obj, af_Core *core);
void gc_addVar(af_Var *obj, af_Core *core);
void gc_addVarSpace(af_VarSpace *obj, af_Core *core);

bool resetGC(af_Core *core);

#endif //AFUN__GC_H_PUBLIC
