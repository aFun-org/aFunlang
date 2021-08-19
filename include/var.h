#ifndef AFUN__VAR_H_PUBLIC
#define AFUN__VAR_H_PUBLIC
#include "macro.h"
#include "object.h"

typedef struct af_Var af_Var;
typedef struct af_VarSpace af_VarSpace;
typedef struct af_VarSpaceListNode af_VarSpaceListNode;

af_Var *makeVar(char *name, char p_self, char p_posterity, char p_external, af_Object *obj);
void freeVar(af_Var *var);
void addVarNode(af_Var var, af_Object *obj, char *id);

af_VarSpace *makeVarSpace(void);
void freeVarSpace(af_VarSpace *vs);

af_VarSpaceListNode *makeVarSpaceList(af_VarSpace *vs);
af_VarSpaceListNode *freeVarSpaceList(af_VarSpaceListNode *vsl);
void freeAllVarSpaceList(af_VarSpaceListNode *vsl);

void addVarSpaceGC(af_VarSpace *vs, af_Core *core);
void addVarGC(af_Var *var, af_Core *core);

bool addVarToVarSpace(af_Var *var, af_VarSpace *vs);
bool makeVarToVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                       af_VarSpace *vs);

af_Var *findVarFromVarSpace(char *name, af_VarSpace *vs);
af_Var *findVarFromVarList(char *name, af_VarSpaceListNode *vsl);

#endif //AFUN__VAR_H_PUBLIC
