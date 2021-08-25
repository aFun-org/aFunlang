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

af_Var *makeVar(char *name, char p_self, char p_external, af_Object *obj, af_Environment *env) {
    af_VarNode *vn = makeVarNode(obj, NULL);
    af_Var *var = calloc(sizeof(af_Var), 1);
    var->name = strCopy(name);
    var->vn = vn;
    var->permissions[0] = p_self;
    var->permissions[1] = p_external;
    gc_addVar(var, env);
    return var;
}

af_Var *makeVarByCore(char *name, char p_self, char p_external, af_Object *obj, af_Core *core) {
    af_VarNode *vn = makeVarNode(obj, NULL);
    af_Var *var = calloc(sizeof(af_Var), 1);
    var->name = strCopy(name);
    var->vn = vn;
    var->permissions[0] = p_self;
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

af_VarSpace *makeVarSpace(af_Environment *env) {
    af_VarSpace *vs = calloc(sizeof(af_VarSpace), 1);
    gc_addVarSpace(vs, env);
    return vs;
}

af_VarSpace *makeVarSpaceByCore(af_Core *core) {
    af_VarSpace *vs = calloc(sizeof(af_VarSpace), 1);
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
 * 若已存在同名Var则返回false不作修改
 * 否则返回true
 */
bool addVarToVarSpace(af_Var *var, af_VarSpace *vs) {
    time33_t index = time33(var->name) % VAR_HASHTABLE_SIZE;
    af_VarCup **pCup = &vs->var[index];

    if (vs->is_protect)
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
 */
bool makeVarToVarSpace(char *name, char p_self, char p_external, af_Object *obj, af_VarSpace *vs, af_Environment *env){
    return addVarToVarSpace(makeVar(name, p_self, p_external, obj, env), vs);
}

bool addVarToVarSpaceList(af_Var *var, af_VarSpaceListNode *vsl) {
    for (NULL; vsl != NULL; vsl = vsl->next) {
        if (!vsl->vs->is_protect)
            return addVarToVarSpace(var, vsl->vs);
    }
    return false;
}

bool makeVarToVarSpaceList(char *name, char p_self, char p_external, af_Object *obj, af_VarSpaceListNode *vsl, af_Environment *env){
    return addVarToVarSpaceList(makeVar(name, p_self, p_external, obj, env), vsl);
}

/*
 * 函数名: findVarFromVarSpaceByIndex
 * 目标: 根据指定的index, 在VarSpace中搜索var
 */
static af_Var *findVarFromVarSpaceByIndex(time33_t index, char *name, af_VarSpace *vs) {
    for (af_VarCup *cup = vs->var[index]; cup != NULL; cup = cup->next) {
        if (EQ_STR(cup->var->name, name))
            return cup->var;
    }
    return NULL;
}

/*
 * 函数名: findVarFromVarSpace
 * 目标: 在VarSpace中搜索var
 */
af_Var *findVarFromVarSpace(char *name, af_VarSpace *vs) {
    return findVarFromVarSpaceByIndex(time33(name) % VAR_HASHTABLE_SIZE, name, vs);
}

/*
 * 函数名: findVarFromVarList
 * 目标: 在VarSpaceListNode中搜索var
 */
af_Var *findVarFromVarList(char *name, af_VarSpaceListNode *vsl) {
    time33_t index = time33(name) % VAR_HASHTABLE_SIZE;
    af_Var *var = NULL;

    for (NULL; vsl != NULL; vsl = vsl->next) {
        var = findVarFromVarSpaceByIndex(index, name, vsl->vs);
        if (var != NULL)
            return var;
    }

    return NULL;
}

af_VarSpaceListNode *pushNewVarList(af_VarSpaceListNode *base, af_Environment *env){
    af_VarSpaceListNode *new = makeVarSpaceList(makeVarSpace(env));
    new->next = base;
    return new;
}

af_VarSpaceListNode *popLastVarList(af_VarSpaceListNode *base) {
    return freeVarSpaceList(base);
}
