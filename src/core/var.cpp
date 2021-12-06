#include "__var.hpp"
#include "core_init.hpp"
#include "tool.hpp"


/* VarNode 创建与释放 */
static af_VarNode *makeVarNode(af_Object *obj, const char *id);
static af_VarNode *freeVarNode(af_VarNode *vn);
static void freeAllVarNode(af_VarNode *vn);

/* VarNode 相关操作 */
static af_VarNode *findVarNode_(af_Var *var, const char *id);

/* VarCup 创建与释放 */
static af_VarCup *makeVarCup(af_Var *var);
static af_VarCup *freeVarCup(af_VarCup *vp);
static void freeAllVarCup(af_VarCup *vp);

/* VarSpace 寻值函数 */
static af_Var *findVarFromVarSpaceByIndex(time33_t index, const char *name, af_VarSpace *vs);

/* 变量权限函数 */
static bool checkVarReadPermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs);
static bool checkVarWritePermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs);

static af_VarNode *makeVarNode(af_Object *obj, const char *id) {
    auto vn = calloc(1, af_VarNode);
    if (id != nullptr)
        vn->id = strCopy(id);
    vn->obj = obj;
    return vn;
}

static af_VarNode *freeVarNode(af_VarNode *vn) {
    auto next = vn->next;
    free(vn->id);
    free(vn);
    return next;
}

static void freeAllVarNode(af_VarNode *vn) {
    while (vn != nullptr)
        vn = freeVarNode(vn);
}

af_Var *makeVar(const char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_Environment *env){
    auto vn = makeVarNode(obj, nullptr);
    auto var = calloc(1, af_Var);
    var->name = strCopy(name);
    var->vn = vn;
    var->permissions[0] = p_self;
    var->permissions[1] = p_posterity;
    var->permissions[2] = p_external;
    pthread_rwlock_init(&var->lock, nullptr);
    gc_addVar(var, env->base);
    return var;
}

void freeVar(af_Var *var, af_Environment *env){
    freeAllVarNode(var->vn);
    free(var->name);
    gc_delVar(var, env);
    pthread_rwlock_destroy(&var->lock);
    free(var);
}

/**
 * 查找指定名字的VarNode
 * 内部函数, 无锁
 * @param var
 * @param id
 * @return
 */
static af_VarNode *findVarNode_(af_Var *var, const char *id) {
    auto vn = var->vn->next;

    if (id == nullptr)
        return var->vn;

    for(NULL; vn != nullptr; vn = vn->next) {
        if (EQ_STR(vn->id, id))
            return vn;
    }
    return nullptr;
}

void addVarNode(af_Var *var, af_Object *obj, const char *id) {
    auto vn = makeVarNode(obj, id);
    pthread_rwlock_wrlock(&var->lock);
    vn->next = var->vn->next;  // 第一个 vn 必须为 id=nullptr
    var->vn->next = vn;
    pthread_rwlock_unlock(&var->lock);
}

/**
 * 查找指定名字的VarNode
 * 外部函数, 带锁
 * 返回值自动添加 gc_addReference
 * @param var
 * @param id
 * @return
 */
af_Object *findVarNode(af_Var *var, const char *id, af_Environment *env){
    pthread_rwlock_rdlock(&var->lock);
    af_VarNode *vn = findVarNode_(var, id);
    af_Object *obj = nullptr;
    if (vn != nullptr) {
        obj = vn->obj;
        gc_addObjectReference(obj, env);
    }
    pthread_rwlock_unlock(&var->lock);
    return obj;
}

static af_VarCup *makeVarCup(af_Var *var) {
    auto vp = calloc(1, af_VarCup);
    vp->var = var;
    return vp;
}

static af_VarCup *freeVarCup(af_VarCup *vp) {
    af_VarCup *next = vp->next;
    free(vp);
    return next;
}

static void freeAllVarCup(af_VarCup *vp) {
    while (vp != nullptr)
        vp = freeVarCup(vp);
}

af_VarSpace *makeVarSpace(af_Object *belong, char p_self, char p_posterity, char p_external, af_Environment *env){
    assertFatalErrorLog(getCoreStatus(env) == core_creat || belong != nullptr, aFunCoreLogger, 1, "makeVarSpace error");
    auto vs = calloc(1, af_VarSpace);
    vs->belong = belong;
    vs->permissions[0] = p_self;
    vs->permissions[1] = p_posterity;
    vs->permissions[2] = p_external;

    pthread_rwlock_init(&vs->lock, nullptr);
    gc_addVarSpace(vs, env->base);
    return vs;
}

void freeVarSpace(af_VarSpace *vs, af_Environment *env) {
    for (int i = 0; i < VAR_HASHTABLE_SIZE; i++)
        freeAllVarCup(vs->var[i]);
    gc_delVarSpace(vs, env);
    pthread_rwlock_destroy(&vs->lock);
    free(vs);
}

af_VarList *makeVarSpaceList(af_VarSpace *vs) {
    auto vsl = calloc(1, af_VarList);
    vsl->vs = vs;
    return vsl;
}

af_VarList *copyVarSpaceList(af_VarList *vsl) {
    af_VarList *base = nullptr;
    af_VarList **pvsl = &base;
    for(NULL; vsl != nullptr; vsl = vsl->next, pvsl = &((*pvsl)->next))
        *pvsl = makeVarSpaceList(vsl->vs);
    return base;
}

static af_VarList *freeVarSpaceList(af_VarList *vsl){
    af_VarList *next = vsl->next;
    free(vsl);
    return next;
}

void freeAllVarSpaceList(af_VarList *vsl){
    while (vsl != nullptr)
        vsl = freeVarSpaceList(vsl);
}

af_VarSpace *getVarSpaceFromList(af_VarList *vsl) {
    if (vsl != nullptr)
        return vsl->vs;
    return nullptr;
}

bool freeVarSpaceListCount(size_t count, af_VarList *vsl) {
    for (size_t i = count; i > 0; i--) {
        if (vsl == nullptr)  // 发生了错误
            return false;
        vsl = freeVarSpaceList(vsl);
    }
    return true;
}

/**
 * 检查访问者是否有定义变量的权限
 * 注意: 无锁, 由调用者加锁
 * @param visitor 访问者
 * @param vs 变量空间
 */
static bool checkVarSpaceDefinePermissions(af_Object *visitor, af_VarSpace *vs){
    char p = vs->permissions[2];  // 默认外部权限

    af_ObjectData *belong_data = nullptr;
    if (vs->belong != nullptr) {
        pthread_rwlock_rdlock(&vs->belong->lock);
        belong_data = vs->belong->data;
        pthread_rwlock_unlock(&vs->belong->lock);
    }

    af_ObjectData *visitor_data;
    if (visitor != nullptr) {
        pthread_rwlock_rdlock(&visitor->lock);
        visitor_data = visitor->data;
        pthread_rwlock_unlock(&visitor->lock);
    }

    if (vs->belong == nullptr || (visitor != nullptr && belong_data == visitor_data))  // (无权限设定或ObjectData匹配) 应用自身权限
        p = vs->permissions[0];
    else if (visitor != nullptr && checkPosterity(vs->belong, visitor))  // 应用后代权限
        p = vs->permissions[1];

    return p == 2 || p == 3;
}

/**
 * 把var添加到VarSpace中
 * 注意: Var必须添加 gc_addReference
 * @param var 变量
 * @param visitor 访问者
 * @param vs 变量空间
 * @return 若空间被保护, 权限错误或已存在同名Var则返回false不作修改 否则返回true
 */
bool addVarToVarSpace(af_Var *var, af_Object *visitor, af_VarSpace *vs, af_Environment *env){
    pthread_rwlock_rdlock(&var->lock);
    pthread_rwlock_wrlock(&vs->lock);

    time33_t index = time33(var->name) % VAR_HASHTABLE_SIZE;
    af_VarCup **pCup = &vs->var[index];

    if (vs->is_protect)
        goto RETURN_FALSE;

    if (!checkVarSpaceDefinePermissions(visitor, vs))
        goto RETURN_FALSE;

    for(NULL; *pCup != nullptr; pCup = &((*pCup)->next)) {
        pthread_rwlock_wrlock(&(*pCup)->var->lock);
        bool res = EQ_STR((*pCup)->var->name, var->name);
        pthread_rwlock_unlock(&(*pCup)->var->lock);

        if (res)
            goto RETURN_FALSE;
    }

    *pCup = makeVarCup(var);
    pthread_rwlock_unlock(&vs->lock);
    pthread_rwlock_unlock(&var->lock);
    gc_delVarReference(var, env);
    return true;

RETURN_FALSE:
    pthread_rwlock_unlock(&vs->lock);
    pthread_rwlock_unlock(&var->lock);
    return false;
}

/**
 * 创建一个新的var添加到VarSpace中
 * 若已存在同名Var则返回false不作修改
 * 否则返回true
 * 调用 addVarToVarSpace
 * 注意: 必须保证 VarSpace被 gc 引用
 */
bool makeVarToVarSpace(const char *name, char p_self, char p_posterity, char p_external, af_Object *obj, af_VarSpace *vs,
                       af_Object *visitor, af_Environment *env){
    af_Var *var = makeVar(name, p_self, p_posterity, p_external, obj, env);
    if (addVarToVarSpace(var, visitor, vs, env))
        return true;
    gc_delVarReference(var, env);
    return false;
}

/*
 * 函数名: makeVarToVarSpace
 * 目标: 添加一个Var到VarSpaceList
 * 自动跳过保护空间
 * 调用 addVarToVarSpace
 */
bool addVarToVarSpaceList(af_Var *var, af_Object *visitor, af_VarList *vsl, af_Environment *env){
    for(NULL; vsl != nullptr; vsl = vsl->next) {
        if (!vsl->vs->is_protect)
            return addVarToVarSpace(var, visitor, vsl->vs, env);
    }
    return false;
}

/**
 * 创建一个新的var到VarSpaceList
 * 自动跳过保护空间
 * 调用 addVarToVarSpaceList -> addVarToVarSpace
 * 注意: 必须保证 VarSpace被 gc 引用
 */
bool makeVarToVarSpaceList(const char *name, char p_self, char p_posterity, char p_external, af_Object *obj,
                           af_VarList *vsl, af_Object *visitor, af_Environment *env){
    af_Var *var = makeVar(name, p_self, p_posterity, p_external, obj, env);
    if (addVarToVarSpaceList(var, visitor, vsl, env))
        return true;
    gc_delVarReference(var, env);
    return false;
}

/**
 * 创建一个新的var变量添加到保护空间中
 * 若已存在同名Var则返回false不作修改
 * 否则返回true
 * 调用 addVarToVarSpace
 */
bool makeVarToProtectVarSpace(const char *name,
                              char p_self, char p_posterity, char p_external,
                              af_Object *obj, af_Environment *env){
    pthread_rwlock_wrlock(&env->protect->lock);
    bool is_protect = env->protect->is_protect;
    env->protect->is_protect = false;
    pthread_rwlock_unlock(&env->protect->lock);

    af_Var *var = makeVar(name, p_self, p_posterity, p_external, obj, env);
    bool ret = addVarToVarSpace(var, env->activity->belong, env->protect, env);
    if (!ret)
        gc_delVarReference(var, env);

    pthread_rwlock_wrlock(&env->protect->lock);
    env->protect->is_protect = is_protect;
    pthread_rwlock_unlock(&env->protect->lock);
    return ret;
}

/*
 * 函数名: addVarToProtectVarSpace
 * 目标: 添加一个var变量添加到保护空间中 (Var需要提前gc_addReference)
 * 若已存在同名Var则返回false不作修改
 * 否则返回true
 * 调用 addVarToVarSpace
 */
bool addVarToProtectVarSpace(af_Var *var, af_Environment *env) {
    pthread_rwlock_wrlock(&env->protect->lock);
    bool is_protect = env->protect->is_protect;
    env->protect->is_protect = false;
    pthread_rwlock_unlock(&env->protect->lock);

    bool re = addVarToVarSpace(var, nullptr, env->protect, env);

    pthread_rwlock_wrlock(&env->protect->lock);
    env->protect->is_protect = is_protect;
    pthread_rwlock_unlock(&env->protect->lock);
    return re;
}


/**
 * 检查访问者是否有删除变量的权限
 * 注意: 无锁, 由调用者加锁
 * @param visitor 访问者
 * @param vs 变量空间
 * @return
 */
static bool checkVarSpaceDelPermissions(af_Object *visitor, af_VarSpace *vs) {
    char p = vs->permissions[2];  // 默认外部权限

    af_ObjectData *belong_data = nullptr;
    if (vs->belong != nullptr) {
        pthread_rwlock_rdlock(&vs->belong->lock);
        belong_data = vs->belong->data;
        pthread_rwlock_unlock(&vs->belong->lock);
    }

    af_ObjectData *visitor_data;
    if (visitor != nullptr) {
        pthread_rwlock_rdlock(&visitor->lock);
        visitor_data = visitor->data;
        pthread_rwlock_unlock(&visitor->lock);
    }

    if (vs->belong == nullptr || (visitor != nullptr && belong_data == visitor_data))  // (无权限设定或ObjectData匹配) 应用自身权限
        p = vs->permissions[0];
    else if (visitor != nullptr && checkPosterity(vs->belong, visitor))  // 应用后代权限
        p = vs->permissions[1];

    return p == 1 || p == 3;
}

/*
 * 函数名: delVarFromVarSpace
 * 目标: 从VarSpace中删除指定的变量
 * 若空间被保护, 权限错误或已存在同名Var则返回false不作修改
 * 否则返回true
 */
bool delVarFromVarSpace(const char *name, af_Object *visitor, af_VarSpace *vs) {
    time33_t index = time33(name) % VAR_HASHTABLE_SIZE;

    pthread_rwlock_wrlock(&vs->lock);
    af_VarCup **pCup = &vs->var[index];

    if (vs->is_protect)
        goto RETRUN_FALSE;

    if (!checkVarSpaceDelPermissions(visitor, vs))
        goto RETRUN_FALSE;

    for(NULL; *pCup != nullptr; pCup = &((*pCup)->next)) {
        pthread_rwlock_wrlock(&(*pCup)->var->lock);

        if (EQ_STR((*pCup)->var->name, name)) {
            *pCup = freeVarCup(*pCup);
            pthread_rwlock_unlock(&(*pCup)->var->lock);
            pthread_rwlock_unlock(&vs->lock);
            return true;
        }

        pthread_rwlock_unlock(&(*pCup)->var->lock);
    }

RETRUN_FALSE:
    pthread_rwlock_unlock(&vs->lock);
    return false;
}

/*
 * 函数名: delVarFromVarList
 * 目标: 从VarSpaceList中第一层的VarSpace中删除指定的变量
 * 若空间被保护, 权限错误或已存在同名Var则返回false不作修改
 * 否则返回true
 * 调用delVarFromVarSpace
 */
bool delVarFromVarList(const char *name, af_Object *visitor, af_VarList *vsl) {
    return delVarFromVarSpace(name, visitor, vsl->vs);
}

/*
 * 函数名: findVarFromVarSpaceByIndex
 * 目标: 根据指定的index, 在VarSpace中搜索var
 * 权限检查交给 findVarFromVarSpace 和 findVarFromVarList
 */
static af_Var *findVarFromVarSpaceByIndex(time33_t index, const char *name, af_VarSpace *vs) {
    pthread_rwlock_rdlock(&vs->lock);
    for (af_VarCup *cup = vs->var[index]; cup != nullptr; cup = cup->next) {
        pthread_rwlock_wrlock(&cup->var->lock);

        if (EQ_STR(cup->var->name, name)) {
            af_Var *var = cup->var;
            pthread_rwlock_unlock(&cup->var->lock);
            pthread_rwlock_unlock(&vs->lock);
            return var;
        }

        pthread_rwlock_unlock(&cup->var->lock);
    }
    pthread_rwlock_unlock(&vs->lock);
    return nullptr;
}

/**
 * 检查访问者是否有读取变量的权限
 * 注意: 会给VarSpace和Var加读锁
 * @param var 变量
 * @param visitor 访问者
 * @param vs  变量空间
 * @return 返回是否具有读取权限
 */
static bool checkVarReadPermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs){
    char p = var->permissions[2];  // 默认外部权限

    pthread_rwlock_rdlock(&vs->lock);
    pthread_rwlock_rdlock(&var->lock);

    af_ObjectData *belong_data = nullptr;
    if (vs->belong != nullptr) {
        pthread_rwlock_rdlock(&vs->belong->lock);
        belong_data = vs->belong->data;
        pthread_rwlock_unlock(&vs->belong->lock);
    }

    af_ObjectData *visitor_data;
    if (visitor != nullptr) {
        pthread_rwlock_rdlock(&visitor->lock);
        visitor_data = visitor->data;
        pthread_rwlock_unlock(&visitor->lock);
    }

    if (vs->belong == nullptr || (visitor != nullptr && belong_data == visitor_data))  // (无权限设定或ObjectData匹配) 应用自身权限
        p = var->permissions[0];
    else if (visitor != nullptr && checkPosterity(vs->belong, visitor))  // 应用后代权限
        p = var->permissions[1];

    pthread_rwlock_unlock(&vs->lock);
    pthread_rwlock_unlock(&var->lock);
    return p == 1 || p == 3;
}

/*
 * 函数名: findVarFromVarSpace
 * 目标: 在VarSpace中搜索var
 * 调用: findVarFromVarSpaceByIndex
 */
af_Var *findVarFromVarSpace(const char *name, af_Object *visitor, af_VarSpace *vs){
    af_Var *var = findVarFromVarSpaceByIndex(time33(name) % VAR_HASHTABLE_SIZE, name, vs);
    if (var == nullptr)
        return nullptr;

    if (checkVarReadPermissions(var, visitor, vs))
        return var;
    return nullptr;
}

/*
 * 函数名: findVarFromVarList
 * 目标: 在VarSpaceListNode中搜索var
 * 调用: findVarFromVarSpaceByIndex
 */
af_Var *findVarFromVarList(const char *name, af_Object *visitor, af_VarList *vsl) {
    time33_t index = time33(name) % VAR_HASHTABLE_SIZE;

    for(NULL; vsl != nullptr; vsl = vsl->next) {
        af_Var *var = findVarFromVarSpaceByIndex(index, name, vsl->vs);
        if (var != nullptr) {
            if (checkVarReadPermissions(var, visitor, vsl->vs))
                return var;
            return nullptr;
        }
    }

    return nullptr;
}

/**
 * 检查访问者是否有改写变量的权限
 * 注意: 不会给VarList加锁, 给Var加读锁, 由调用者加锁
 * @param var 变量
 * @param visitor 访问者
 * @param vs 变量空间
 * @return
 */
static bool checkVarWritePermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs){
    char p = var->permissions[2];  // 默认外部权限

    af_ObjectData *belong_data = nullptr;
    if (vs->belong != nullptr) {
        pthread_rwlock_rdlock(&vs->belong->lock);
        belong_data = vs->belong->data;
        pthread_rwlock_unlock(&vs->belong->lock);
    }

    af_ObjectData *visitor_data;
    if (visitor != nullptr) {
        pthread_rwlock_rdlock(&visitor->lock);
        visitor_data = visitor->data;
        pthread_rwlock_unlock(&visitor->lock);
    }

    pthread_rwlock_rdlock(&vs->lock);
    if (vs->belong == nullptr || (visitor != nullptr && belong_data == visitor_data))  // (无权限设定或ObjectData匹配) 应用自身权限
        p = var->permissions[0];
    else if (visitor != nullptr && checkPosterity(vs->belong, visitor))  // 应用后代权限
        p = var->permissions[1];
    pthread_rwlock_unlock(&vs->lock);

    return p == 2 || p == 3;
}

/*
 * 函数名: setVarToVarSpace
 * 目标: 在VarSpace中搜索var并修改其值
 * 调用: findVarFromVarSpaceByIndex
 */
bool setVarToVarSpace(const char *name, af_Object *obj, af_Object *visitor, af_VarSpace *vs){
    af_Var *var = findVarFromVarSpaceByIndex(time33(name) % VAR_HASHTABLE_SIZE, name, vs);
    if (var == nullptr)
        return false;

    pthread_rwlock_wrlock(&vs->lock);
    if (checkVarWritePermissions(var, visitor, vs)) {
        pthread_rwlock_unlock(&vs->lock);

        pthread_rwlock_wrlock(&var->lock);
        var->vn->obj = obj;
        pthread_rwlock_unlock(&var->lock);
        return true;
    }

    pthread_rwlock_unlock(&vs->lock);
    return false;
}

/*
 * 函数名: setVarToVarList
 * 目标: 在VarSpaceListNode中搜索var并修改其值
 * 调用: findVarFromVarSpaceByIndex
 */
bool setVarToVarList(const char *name, af_Object *obj, af_Object *visitor, af_VarList *vsl) {
    time33_t index = time33(name) % VAR_HASHTABLE_SIZE;
    af_Var *var;

    for(NULL; vsl != nullptr; vsl = vsl->next) {
        var = findVarFromVarSpaceByIndex(index, name, vsl->vs);
        if (var != nullptr) {
            pthread_rwlock_wrlock(&vsl->vs->lock);
            if (checkVarWritePermissions(var, visitor, vsl->vs)) {
                pthread_rwlock_unlock(&vsl->vs->lock);

                pthread_rwlock_wrlock(&var->lock);
                var->vn->obj = obj;
                pthread_rwlock_unlock(&var->lock);

                return true;
            }
            pthread_rwlock_unlock(&vsl->vs->lock);
            return false;
        }
    }
    return false;
}

/**
 * 压入新的空VarSpace
 * 并且保留gc计数
 * @param belong
 * @param base
 * @param env
 * @return
 */
af_VarList *pushNewVarList(af_Object *belong, af_VarList *base, af_Environment *env){
    af_VarSpace *vs = makeVarSpace(belong, 3, 2, 0, env);
    af_VarList *new_vs = makeVarSpaceList(vs);
    new_vs->next = base;
    return new_vs;
}

/**
 * 压入新的空保护空间的VarSpace
 * @param belong
 * @param base
 * @param env
 * @return
 */
af_VarList *pushProtectVarList(af_VarList *base, af_Environment *env){
    af_VarList *new_vsl = makeVarSpaceList(getProtectVarSpace(env));
    new_vsl->next = base;
    return new_vsl;
}

af_VarList *pushVarList(af_VarSpace *vs, af_VarList *base){
    af_VarList *new_vsl = makeVarSpaceList(vs);
    new_vsl->next = base;
    return new_vsl;
}

void setVarPermissions(af_Var *var, af_Object *visitor, af_VarSpace *vs, char p_self, char p_posterity, char p_external) {
    pthread_rwlock_rdlock(&vs->lock);

    af_ObjectData *belong_data = nullptr;
    if (vs->belong != nullptr) {
        pthread_rwlock_rdlock(&vs->belong->lock);
        belong_data = vs->belong->data;
        pthread_rwlock_unlock(&vs->belong->lock);
    }

    af_ObjectData *visitor_data = nullptr;
    if (visitor != nullptr) {
        pthread_rwlock_rdlock(&visitor->lock);
        visitor_data = visitor->data;
        pthread_rwlock_unlock(&visitor->lock);
    }

    pthread_rwlock_unlock(&vs->lock);

    if (belong_data != nullptr && belong_data != visitor_data)
        return;

    pthread_rwlock_wrlock(&var->lock);
    var->permissions[0] = p_self;
    var->permissions[1] = p_posterity;
    var->permissions[2] = p_external;
    pthread_rwlock_unlock(&var->lock);
}

void setVarSpacePermissions(af_Object *visitor, af_VarSpace *vs, char p_self, char p_posterity, char p_external) {
    pthread_rwlock_rdlock(&vs->lock);

    af_ObjectData *belong_data = nullptr;
    if (vs->belong != nullptr) {
        pthread_rwlock_rdlock(&vs->belong->lock);
        belong_data = vs->belong->data;
        pthread_rwlock_unlock(&vs->belong->lock);
    }

    af_ObjectData *visitor_data = nullptr;
    if (visitor != nullptr) {
        pthread_rwlock_rdlock(&visitor->lock);
        visitor_data = visitor->data;
        pthread_rwlock_unlock(&visitor->lock);
    }

    pthread_rwlock_unlock(&vs->lock);

    if (belong_data != nullptr && belong_data != visitor_data)
        return;

    vs->permissions[0] = p_self;
    vs->permissions[1] = p_posterity;
    vs->permissions[2] = p_external;
}

bool isProtectVarSpace(af_VarSpace *vs) {
    pthread_rwlock_rdlock(&vs->lock);
    bool res = vs->is_protect;
    pthread_rwlock_unlock(&vs->lock);
    return res;
}

bool setVarSpaceProtect(af_Object *visitor, af_VarSpace *vs, bool protect) {
    pthread_rwlock_wrlock(&vs->lock);
    bool re = vs->is_protect;

    af_ObjectData *belong_data = nullptr;
    if (vs->belong != nullptr) {
        pthread_rwlock_rdlock(&vs->belong->lock);
        belong_data = vs->belong->data;
        pthread_rwlock_unlock(&vs->belong->lock);
    }

    af_ObjectData *visitor_data = nullptr;
    if (visitor != nullptr) {
        pthread_rwlock_rdlock(&visitor->lock);
        visitor_data = visitor->data;
        pthread_rwlock_unlock(&visitor->lock);
    }

    if (belong_data != nullptr && belong_data != visitor_data) {
        pthread_rwlock_unlock(&vs->lock);
        return re;
    }

    vs->is_protect = protect;
    pthread_rwlock_unlock(&vs->lock);
    return re;
}