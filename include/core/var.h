#ifndef AFUN_VAR
#define AFUN_VAR

typedef struct af_Var af_Var;
typedef struct af_VarSpace af_VarSpace;
typedef struct af_VarSpaceListNode af_VarSpaceListNode;

#include "aFunCoreExport.h"
#include "tool.h"
#include "object.h"

/* 变量创建与释放 */
AFUN_CORE_EXPORT af_Var *makeVar(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_Environment *env);

/* 变量值管理 */
AFUN_CORE_EXPORT void addVarNode(af_Var var, af_Object *obj, char *id);

/* 变量空间创建与释放 */
AFUN_CORE_EXPORT af_VarSpace *makeVarSpace(af_Object *belong, char p_self, char p_posterity, char p_external, af_Environment *env);

/* 变量定义函数 */
AFUN_CORE_EXPORT bool addVarToVarSpace(af_Var *var, af_Object *visitor, af_VarSpace *vs);
AFUN_CORE_EXPORT bool makeVarToVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_VarSpace *vs,
                                        af_Object *visitor, af_Environment *env);
AFUN_CORE_EXPORT bool addVarToVarSpaceList(af_Var *var, af_Object *visitor, af_VarSpaceListNode *vsl);
AFUN_CORE_EXPORT bool makeVarToVarSpaceList(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                                            af_VarSpaceListNode *vsl, af_Object *visitor, af_Environment *env);
AFUN_CORE_EXPORT bool makeVarToProtectVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                                               af_Environment *env);

/* 变量删除函数 */
AFUN_CORE_EXPORT bool delVarFromVarSpace(char *name, af_Object *visitor, af_VarSpace *vs);
AFUN_CORE_EXPORT bool delVarFromVarList(char *name, af_Object *visitor, af_VarSpaceListNode *vsl);

/* 变量访问函数 */
AFUN_CORE_EXPORT af_Var *findVarFromVarSpace(char *name, af_Object *visitor, af_VarSpace *vs);
AFUN_CORE_EXPORT af_Var *findVarFromVarList(char *name, af_Object *visitor, af_VarSpaceListNode *vsl);

/* 变量设置函数 */
AFUN_CORE_EXPORT bool setVarToVarSpace(char *name, af_Object *obj, af_Object *visitor, af_VarSpace *vs);
AFUN_CORE_EXPORT bool setVarToVarList(char *name, af_Object *obj, af_Object *visitor, af_VarSpaceListNode *vsl);

/* 变量空间链的创建与释放 */
AFUN_CORE_EXPORT af_VarSpaceListNode *makeVarSpaceList(af_VarSpace *vs);
AFUN_CORE_EXPORT void freeAllVarSpaceList(af_VarSpaceListNode *vsl);
AFUN_CORE_EXPORT bool freeVarSpaceListCount(size_t count, af_VarSpaceListNode *vsl);

/* 变量空间链属性获取 */
AFUN_CORE_EXPORT af_VarSpace *getVarSpaceFromList(af_VarSpaceListNode *vsl);

/* 变量空间链管理 */
AFUN_CORE_EXPORT af_VarSpaceListNode *pushNewVarList(af_Object *belong, af_VarSpaceListNode *base, af_Environment *env);

#endif //AFUN_VAR
