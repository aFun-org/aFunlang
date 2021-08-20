#include "__var.h"
#include "__env.h"
#include "tool.h"

static af_VarNode *makeVarNode(af_Object *obj, char *id);
static af_VarNode *freeVarNode(af_VarNode *vn);
static void freeAllVarNode(af_VarNode *vn);

static af_VarCup *makeVarCup(af_Var *var);
static af_VarCup *freeVarCup(af_VarCup *vp);
static void freeAllVarCup(af_VarCup *vp);
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

af_Var *makeVar(char *name, char p_self, char p_posterity, char p_external, af_Object *obj) {
    af_VarNode *vn = makeVarNode(obj, NULL);
    af_Var *var = calloc(sizeof(af_Var), 1);
    var->name = strCopy(name);
    var->vn = vn;
    var->permissions[0] = p_self;
    var->permissions[1] = p_posterity;
    var->permissions[2] = p_external;
    return var;
}

void freeVar(af_Var *var) {
    freeAllVarNode(var->vn);
    free(var->name);
    if (var->gc.info.start_gc) {
        GC_FREE_EXCHANGE(var);
    }
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
    if (!vp->var->gc.info.start_gc)
        freeVar(vp->var);
    free(vp);
    return next;
}

static void freeAllVarCup(af_VarCup *vp) {
    while (vp != NULL)
        vp = freeVarCup(vp);
}

af_VarSpace *makeVarSpace(void) {
    af_VarSpace *vs = calloc(sizeof(af_VarSpace), 1);
    return vs;
}

void freeVarSpace(af_VarSpace *vs) {
    for (int i = 0; i < VAR_HASHTABLE_SIZE; i++)
        freeAllVarCup(vs->var[i]);
    if (vs->gc.info.start_gc) {
        GC_FREE_EXCHANGE(vs);
    }
    free(vs);
}

af_VarSpaceListNode *makeVarSpaceList(af_VarSpace *vs) {
    af_VarSpaceListNode *vsl = calloc(sizeof(af_VarSpaceListNode), 1);
    vsl->vs = vs;
    return vsl;
}

af_VarSpaceListNode *freeVarSpaceList(af_VarSpaceListNode *vsl) {
    af_VarSpaceListNode *next = vsl->next;
    if (!vsl->vs->gc.info.start_gc)
        freeVarSpace(vsl->vs);
    free(vsl);
    return next;
}

void freeAllVarSpaceList(af_VarSpaceListNode *vsl) {
    while (vsl != NULL)
        vsl = freeVarSpaceList(vsl);
}

void addVarSpaceGCByCore(af_VarSpace *vs, af_Core *core) {
    if (vs->gc.info.start_gc)
        return;

    vs->gc.info.start_gc = true;
    gc_addVarSpaceByCore(vs, core);
}

void addVarSpaceGC(af_VarSpace *vs, af_Environment *env) {
    if (vs->gc.info.start_gc)
        return;

    vs->gc.info.start_gc = true;
    gc_addVarSpace(vs, env);
}

void addVarGCByCore(af_Var *var, af_Core *core) {
    if (var->gc.info.start_gc)
        return;

    var->gc.info.start_gc = true;
    gc_addVarByCore(var, core);
}

void addVarGC(af_Var *var, af_Environment *env) {
    if (var->gc.info.start_gc)
        return;

    var->gc.info.start_gc = true;
    gc_addVar(var, env);
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
bool makeVarToVarSpace(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                       af_VarSpace *vs) {
    return addVarToVarSpace(makeVar(name, p_self, p_posterity, p_external, obj), vs);
}

bool addVarToVarSpaceList(af_Var *var, af_VarSpaceListNode *vsl) {
    for (NULL; vsl != NULL; vsl = vsl->next) {
        if (!vsl->vs->is_protect)
            return addVarToVarSpace(var, vsl->vs);
    }
    return false;
}

bool makeVarToVarSpaceList(char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                           af_VarSpaceListNode *vsl) {
    return addVarToVarSpaceList(makeVar(name, p_self, p_posterity, p_external, obj), vsl);
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

af_VarSpaceListNode *pushNewVarList(af_VarSpaceListNode *base) {
    af_VarSpaceListNode *new = makeVarSpaceList(makeVarSpace());
    new->next = base;
    return new;
}

af_VarSpaceListNode *popLastVarList(af_VarSpaceListNode *base) {
    return freeVarSpaceList(base);
}
