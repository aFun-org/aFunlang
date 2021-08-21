#ifndef AFUN__VAR_H_PUBLIC
#define AFUN__VAR_H_PUBLIC

typedef struct af_Var af_Var;
typedef struct af_VarSpace af_VarSpace;
typedef struct af_VarSpaceListNode af_VarSpaceListNode;

#include "macro.h"
#include "object.h"

af_Var *makeVar(char *name, char p_self, char p_posterity, char p_external, af_Object *obj);
void freeVar(af_Var *var);
void addVarNode(af_Var var, af_Object *obj, char *id);

af_VarSpace *makeVarSpace(void);
void freeVarSpace(af_VarSpace *vs);

af_VarSpaceListNode *makeVarSpaceList(af_VarSpace *vs);
af_VarSpaceListNode *freeVarSpaceList(af_VarSpaceListNode *vsl);
void freeAllVarSpaceList(af_VarSpaceListNode *vsl);

void addVarSpaceGC(af_VarSpace *vs, af_Environment *env);
void addVarGC(af_Var *var, af_Environment *env);

bool addVarToVarSpace(af_Var *var, af_VarSpace *vs);
bool makeVarToVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                       af_VarSpace *vs);

bool addVarToVarSpaceList(af_Var *var, af_VarSpaceListNode *vsl);
bool makeVarToVarSpaceList(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                           af_VarSpaceListNode *vsl);

af_Var *findVarFromVarSpace(char *name, af_VarSpace *vs);
af_Var *findVarFromVarList(char *name, af_VarSpaceListNode *vsl);

af_VarSpaceListNode *pushNewVarList(af_VarSpaceListNode *base);
af_VarSpaceListNode *popLastVarList(af_VarSpaceListNode *base);

#endif //AFUN__VAR_H_PUBLIC
