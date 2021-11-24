#ifndef AFUN_VAR
#define AFUN_VAR

typedef struct af_Var af_Var;
typedef struct af_VarSpace af_VarSpace;
typedef struct af_VarList af_VarList;

#include "aFunCoreExport.h"
#include "tool.h"
#include "object.h"

/* 变量 创建 */
AFUN_CORE_EXPORT af_Var *makeVar(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_Environment *env);

/* 变量空间 创建 */
AFUN_CORE_EXPORT af_VarSpace *makeVarSpace(af_Object *belong, char p_self, char p_posterity, char p_external, af_Environment *env);

/* 变量空间链 创建与释放 */
AFUN_CORE_EXPORT af_VarList *makeVarSpaceList(af_VarSpace *vs);
AFUN_CORE_EXPORT af_VarList *copyVarSpaceList(af_VarList *vsl);
AFUN_CORE_EXPORT void freeAllVarSpaceList(af_VarList *vsl);
AFUN_CORE_EXPORT bool freeVarSpaceListCount(size_t count, af_VarList *vsl);

/* 变量 相关操作 */
AFUN_CORE_EXPORT void addVarNode(af_Var *var, af_Object *obj, char *id);

/* 变量空间 相关操作 */
AFUN_CORE_EXPORT bool addVarToVarSpace(af_Var *var, af_Object *visitor, af_VarSpace *vs, af_Environment *env);
AFUN_CORE_EXPORT bool makeVarToVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_VarSpace *vs,
                                        af_Object *visitor, af_Environment *env);
AFUN_CORE_EXPORT bool delVarFromVarSpace(char *name, af_Object *visitor, af_VarSpace *vs);
AFUN_CORE_EXPORT bool setVarToVarSpace(char *name, af_Object *obj, af_Object *visitor, af_VarSpace *vs);
AFUN_CORE_EXPORT void setVarPermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs, char p_self, char p_posterity, char p_external);
AFUN_CORE_EXPORT void setVarSpacePermissions(af_Object *visitor, af_VarSpace *vs, char p_self, char p_posterity, char p_external);
AFUN_CORE_EXPORT bool setVarSpaceProtect(af_Object *visitor, af_VarSpace *vs, bool protect);

/* 变量空间链 相关操作 */
AFUN_CORE_EXPORT bool addVarToVarSpaceList(af_Var *var, af_Object *visitor, af_VarList *vsl,
                                           af_Environment *env);
AFUN_CORE_EXPORT bool makeVarToVarSpaceList(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                                            af_VarList *vsl, af_Object *visitor, af_Environment *env);
AFUN_CORE_EXPORT bool makeVarToProtectVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                                               af_Environment *env);
AFUN_CORE_EXPORT bool addVarToProtectVarSpace(af_Var *var, af_Environment *env);
AFUN_CORE_EXPORT bool delVarFromVarList(char *name, af_Object *visitor, af_VarList *vsl);
AFUN_CORE_EXPORT bool setVarToVarList(char *name, af_Object *obj, af_Object *visitor, af_VarList *vsl);
AFUN_CORE_EXPORT af_VarList *pushNewVarList(af_Object *belong, af_VarList *base, af_Environment *env);
AFUN_CORE_EXPORT af_VarList *pushProtectVarList(af_VarList *base, af_Environment *env);
AFUN_CORE_EXPORT af_VarList *pushVarList(af_VarSpace *vs, af_VarList *base);

/* 变量 属性访问 */
AFUN_CORE_EXPORT af_Object *findVarNode(af_Var *var, char *id, af_Environment *env);

/* 变量空间 属性访问 */
AFUN_CORE_EXPORT af_Var *findVarFromVarSpace(char *name, af_Object *visitor, af_VarSpace *vs);
AFUN_CORE_EXPORT bool isProtectVarSpace(af_VarSpace *vs);

/* 变量空间链 属性访问 */
AFUN_CORE_EXPORT af_Var *findVarFromVarList(char *name, af_Object *visitor, af_VarList *vsl);
AFUN_CORE_EXPORT af_VarSpace *getVarSpaceFromList(af_VarList *vsl);

#endif //AFUN_VAR
