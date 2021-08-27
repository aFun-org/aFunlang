#ifndef AFUN__VAR_H_PUBLIC
#define AFUN__VAR_H_PUBLIC

typedef struct af_Var af_Var;
typedef struct af_VarSpace af_VarSpace;
typedef struct af_VarSpaceListNode af_VarSpaceListNode;

#include "macro.h"
#include "object.h"

/* 变量创建与释放 */
af_Var *makeVar(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_Environment *env);
void freeVar(af_Var *var, af_Environment *env);

/* 变量值管理 */
void addVarNode(af_Var var, af_Object *obj, char *id);

/* 变量空间创建与释放 */
af_VarSpace *makeVarSpace(af_Object *belong, af_Environment *env);
void freeVarSpace(af_VarSpace *vs, af_Environment *env);

/* 变量定义函数 */
bool addVarToVarSpace(af_Var *var, af_Object *visitor, af_VarSpace *vs);
bool makeVarToVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_VarSpace *vs,
                       af_Object *visitor, af_Environment *env);
bool addVarToVarSpaceList(af_Var *var, af_Object *visitor, af_VarSpaceListNode *vsl);
bool makeVarToVarSpaceList(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                           af_VarSpaceListNode *vsl, af_Object *visitor, af_Environment *env);

/* 变量删除函数 */
bool delVarFromVarSpace(char *name, af_Object *visitor, af_VarSpace *vs);
bool delVarFromVarList(char *name, af_Object *visitor, af_VarSpaceListNode *vsl);

/* 变量访问函数 */
af_Var *findVarFromVarSpace(char *name, af_Object *visitor, af_VarSpace *vs);
af_Var *findVarFromVarList(char *name, af_Object *visitor, af_VarSpaceListNode *vsl);

/* 变量设置函数 */
bool setVarToVarSpace(char *name, af_Object *obj, af_Object *visitor, af_VarSpace *vs);
bool setVarToVarList(char *name, af_Object *obj, af_Object *visitor, af_VarSpaceListNode *vsl);

/* 变量空间链的创建与释放 */
af_VarSpaceListNode *makeVarSpaceList(af_VarSpace *vs);
af_VarSpaceListNode *freeVarSpaceList(af_VarSpaceListNode *vsl);
void freeAllVarSpaceList(af_VarSpaceListNode *vsl);
bool freeVarSpaceListCount(size_t count, af_VarSpaceListNode *vsl);

/* 变量空间链属性获取 */
af_VarSpace *getVarSpaceFromList(af_VarSpaceListNode *vsl);

/* 变量空间链管理 */
af_VarSpaceListNode *pushNewVarList(af_Object *belong, af_VarSpaceListNode *base, af_Environment *env);
af_VarSpaceListNode *popLastVarList(af_VarSpaceListNode *base);

#endif //AFUN__VAR_H_PUBLIC
