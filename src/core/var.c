#include "__var.h"
#include "__env.h"
#include "tool.h"

/* VarNode 创建与释放 */
static af_VarNode *makeVarNode(af_Object *obj, char *id);
static af_VarNode *freeVarNode(af_VarNode *vn);
static void freeAllVarNode(af_VarNode *vn);

/* VarCup 创建与释放 */
static af_VarCup *makeVarCup(af_Var *var);
static af_VarCup *freeVarCup(af_VarCup *vp);
static void freeAllVarCup(af_VarCup *vp);

/* VarSpace 寻值函数 */
static af_Var *findVarFromVarSpaceByIndex(time33_t index, char *name, af_VarSpace *vs);

/* 变量权限函数 */
static bool checkReadPermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs);
static bool checkWritePermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs);

static af_VarNode *makeVarNode(af_Object *obj, char *id) {
    af_VarNode *vn = calloc(sizeof(af_VarNode), 1);
    if (id != NULL)
        vn->id = strCopy(id);
    vn->obj = obj;
    return vn;
}

static af_VarNode *freeVarNode(af_VarNode *vn) {
    af_VarNode *next = vn->next;
    free(vn->id);
    free(vn);
    return next;
}

static void freeAllVarNode(af_VarNode *vn) {
    while (vn != NULL)
        vn = freeVarNode(vn);
}

af_Var *makeVar(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_Environment *env){
    af_VarNode *vn = makeVarNode(obj, NULL);
    af_Var *var = calloc(sizeof(af_Var), 1);
    var->name = strCopy(name);
    var->vn = vn;
    var->permissions[0] = p_self;
    var->permissions[1] = p_posterity;
    var->permissions[2] = p_external;
    gc_addVar(var, env);
    return var;
}

af_Var *makeVarByCore(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_Core *core){
    af_VarNode *vn = makeVarNode(obj, NULL);
    af_Var *var = calloc(sizeof(af_Var), 1);
    var->name = strCopy(name);
    var->vn = vn;
    var->permissions[0] = p_self;
    var->permissions[1] = p_posterity;
    var->permissions[1] = p_external;
    gc_addVarByCore(var, core);
    return var;
}

void freeVar(af_Var *var, af_Environment *env){
    freeAllVarNode(var->vn);
    free(var->name);
    GC_FREE_EXCHANGE(var, Var, env->core);
    free(var);
}

void freeVarByCore(af_Var *var, af_Core *core) {
    freeAllVarNode(var->vn);
    free(var->name);
    GC_FREE_EXCHANGE(var, Var, core);
    free(var);
}

void addVarNode(af_Var var, af_Object *obj, char *id) {
    af_VarNode *vn = makeVarNode(obj, id);
    vn->next = var.vn->next;  // 第一个 vn 必须表示返回值
    var.vn->next = vn;
}

static af_VarCup *makeVarCup(af_Var *var) {
    af_VarCup *vp = calloc(sizeof(af_VarCup), 1);
    vp->var = var;
    return vp;
}

static af_VarCup *freeVarCup(af_VarCup *vp) {
    af_VarCup *next = vp->next;
    free(vp);
    return next;
}

static void freeAllVarCup(af_VarCup *vp) {
    while (vp != NULL)
        vp = freeVarCup(vp);
}

af_VarSpace *makeVarSpace(af_Object *belong, af_Environment *env) {
    if (!env->core->in_init && belong == NULL)
        return NULL;

    af_VarSpace *vs = calloc(sizeof(af_VarSpace), 1);
    vs->belong = belong;
    gc_addVarSpace(vs, env);
    return vs;
}

af_VarSpace *makeVarSpaceByCore(af_Object *belong, af_Core *core) {
    if (!core->in_init && belong == NULL)
        return NULL;

    af_VarSpace *vs = calloc(sizeof(af_VarSpace), 1);
    vs->belong = belong;
    gc_addVarSpaceByCore(vs, core);
    return vs;
}

void freeVarSpace(af_VarSpace *vs, af_Environment *env) {
    for (int i = 0; i < VAR_HASHTABLE_SIZE; i++)
        freeAllVarCup(vs->var[i]);
    GC_FREE_EXCHANGE(vs, VarSpace, env->core);
    free(vs);
}

void freeVarSpaceByCore(af_VarSpace *vs, af_Core *core) {
    for (int i = 0; i < VAR_HASHTABLE_SIZE; i++)
        freeAllVarCup(vs->var[i]);
    GC_FREE_EXCHANGE(vs, VarSpace, core);
    free(vs);
}

af_VarSpaceListNode *makeVarSpaceList(af_VarSpace *vs) {
    af_VarSpaceListNode *vsl = calloc(sizeof(af_VarSpaceListNode), 1);
    vsl->vs = vs;
    return vsl;
}

af_VarSpaceListNode *freeVarSpaceList(af_VarSpaceListNode *vsl){
    af_VarSpaceListNode *next = vsl->next;
    free(vsl);
    return next;
}

void freeAllVarSpaceList(af_VarSpaceListNode *vsl){
    while (vsl != NULL)
        vsl = freeVarSpaceList(vsl);
}

af_VarSpace *getVarSpaceFromList(af_VarSpaceListNode *vsl) {
    if (vsl != NULL)
        return vsl->vs;
    return NULL;
}

bool freeVarSpaceListCount(size_t count, af_VarSpaceListNode *vsl) {
    for (size_t i = count; i > 0; i--) {
        if (vsl == NULL)  // 发生了错误
            return false;
        vsl = freeVarSpaceList(vsl);
    }
    return true;
}

/*
 * 函数名: addVarToVarSpace
 * 目标: 把var添加到VarSpace中
 * 若空间被保护, 权限错误或已存在同名Var则返回false不作修改
 * 否则返回true
 */
bool addVarToVarSpace(af_Var *var, af_Object *visitor, af_VarSpace *vs) {
    time33_t index = time33(var->name) % VAR_HASHTABLE_SIZE;
    af_VarCup **pCup = &vs->var[index];

    if (vs->is_protect)
        return false;

    if (vs->belong != NULL && (visitor == NULL || visitor->data != vs->belong->data))
        return false;

    for (NULL; *pCup != NULL; pCup = &((*pCup)->next)) {
        if (EQ_STR((*pCup)->var->name, var->name))
            return false;
    }

    *pCup = makeVarCup(var);
    return true;
}

/*
 * 函数名: makeVarToVarSpace
 * 目标: 创建一个新的var添加到VarSpace中
 * 若已存在同名Var则返回false不作修改
 * 否则返回true
 * 调用 addVarToVarSpace
 */
bool makeVarToVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_VarSpace *vs,
                       af_Object *visitor, af_Environment *env){
    return addVarToVarSpace(makeVar(name, p_self, p_posterity, p_external, obj, env), visitor, vs);
}

/*
 * 函数名: makeVarToVarSpace
 * 目标: 添加一个Var到VarSpaceList
 * 自动跳过保护空间
 * 调用 addVarToVarSpace
 */
bool addVarToVarSpaceList(af_Var *var, af_Object *visitor, af_VarSpaceListNode *vsl) {
    for (NULL; vsl != NULL; vsl = vsl->next) {
        if (!vsl->vs->is_protect)
            return addVarToVarSpace(var, visitor, vsl->vs);
    }
    return false;
}

/*
 * 函数名: makeVarToVarSpace
 * 目标: 创建一个新的var到VarSpaceList
 * 自动跳过保护空间
 * 调用 addVarToVarSpaceList -> addVarToVarSpace
 */
bool makeVarToVarSpaceList(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                           af_VarSpaceListNode *vsl, af_Object *visitor, af_Environment *env){
    return addVarToVarSpaceList(makeVar(name, p_self, p_posterity, p_external, obj, env), visitor, vsl);
}

/*
 * 函数名: findVarFromVarSpaceByIndex
 * 目标: 根据指定的index, 在VarSpace中搜索var
 * 权限检查交给 findVarFromVarSpace 和 findVarFromVarList
 */
static af_Var *findVarFromVarSpaceByIndex(time33_t index, char *name, af_VarSpace *vs) {
    for (af_VarCup *cup = vs->var[index]; cup != NULL; cup = cup->next) {
        if (EQ_STR(cup->var->name, name))
            return cup->var;
    }
    return NULL;
}

static bool checkReadPermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs){
    char p = var->permissions[2];  // 默认外部权限

    if (vs->belong == NULL || (visitor != NULL && vs->belong->data == visitor->data))  // (无权限设定或ObjectData匹配) 应用自身权限
        p = var->permissions[0];
    else if (visitor != NULL && checkPosterity(vs->belong, visitor))  // 应用后代权限
        p = var->permissions[1];

    return p == 1 || p == 3;
}

/*
 * 函数名: findVarFromVarSpace
 * 目标: 在VarSpace中搜索var
 * 调用: findVarFromVarSpaceByIndex
 */
af_Var *findVarFromVarSpace(char *name, af_Object *visitor, af_VarSpace *vs){
    af_Var *var = findVarFromVarSpaceByIndex(time33(name) % VAR_HASHTABLE_SIZE, name, vs);
    if (var == NULL)
        return NULL;

    if (checkReadPermissions(var, visitor, vs))
        return var;
    return NULL;
}

/*
 * 函数名: findVarFromVarList
 * 目标: 在VarSpaceListNode中搜索var
 * 调用: findVarFromVarSpaceByIndex
 */
af_Var *findVarFromVarList(char *name, af_Object *visitor, af_VarSpaceListNode *vsl) {
    time33_t index = time33(name) % VAR_HASHTABLE_SIZE;
    af_Var *var = NULL;

    for (NULL; vsl != NULL; vsl = vsl->next) {
        var = findVarFromVarSpaceByIndex(index, name, vsl->vs);
        if (var != NULL) {
            if (checkReadPermissions(var, visitor, vsl->vs))
                return var;
            return NULL;
        }
    }

    return NULL;
}

static bool checkWritePermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs){
    char p = var->permissions[2];  // 默认外部权限

    if (vs->belong == NULL || (visitor != NULL && vs->belong->data == visitor->data))  // (无权限设定或ObjectData匹配) 应用自身权限
        p = var->permissions[0];
    else if (visitor != NULL && checkPosterity(vs->belong, visitor))  // 应用后代权限
        p = var->permissions[1];

    return p == 2 || p == 3;
}

/*
 * 函数名: setVarToVarSpace
 * 目标: 在VarSpace中搜索var并修改其值
 * 调用: findVarFromVarSpaceByIndex
 */
bool setVarToVarSpace(char *name, af_Object *obj, af_Object *visitor, af_VarSpace *vs){
    af_Var *var = findVarFromVarSpaceByIndex(time33(name) % VAR_HASHTABLE_SIZE, name, vs);
    if (var == NULL)
        return false;

    if (checkWritePermissions(var, visitor, vs)) {
        var->vn->obj = obj;
        return true;
    }
    return false;
}

/*
 * 函数名: setVarToVarList
 * 目标: 在VarSpaceListNode中搜索var并修改其值
 * 调用: findVarFromVarSpaceByIndex
 */
bool setVarToVarList(char *name, af_Object *obj, af_Object *visitor, af_VarSpaceListNode *vsl) {
    time33_t index = time33(name) % VAR_HASHTABLE_SIZE;
    af_Var *var = NULL;

    for (NULL; vsl != NULL; vsl = vsl->next) {
        var = findVarFromVarSpaceByIndex(index, name, vsl->vs);
        if (var != NULL) {
            if (checkWritePermissions(var, visitor, vsl->vs)) {
                var->vn->obj = obj;
                return true;
            }
            return false;
        }
    }
    return false;
}

af_VarSpaceListNode *pushNewVarList(af_Object *belong, af_VarSpaceListNode *base, af_Environment *env){
    af_VarSpaceListNode *new = makeVarSpaceList(makeVarSpace(belong, env));
    new->next = base;
    return new;
}

af_VarSpaceListNode *popLastVarList(af_VarSpaceListNode *base) {
    return freeVarSpaceList(base);
}
