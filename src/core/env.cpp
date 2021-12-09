﻿#include "aFunCore.hpp"
#include "__env.hpp"
#include "__global_obj.hpp"
#include "__run.hpp"
#include "__sig.hpp"
#include "obj_api.h"

/* Activity 创建和释放 */
static af_Activity *makeActivity(af_Message *msg_up, af_VarList *varlist, af_Object *belong, af_Environment *env);
static af_Activity *makeFuncActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                     af_VarList *out_varlist, af_Object *belong, af_Object *func,
                                     af_Environment *env);
static af_Activity * makeTopActivity(af_Code *bt_top, af_Code *bt_start, af_VarSpace *protect, af_Object *belong,
                                     af_Environment *env);
static af_Activity *makeTopImportActivity(af_Code *bt_top, af_Code *bt_start, af_VarSpace *protect, af_Object *belong,
                                          const char *mark, af_Environment *env);
static af_Activity *makeGuardianActivity(af_GuardianList *gl, af_GuardianList **pgl, af_Environment *env);
static af_Activity *freeActivity(af_Activity *activity, af_Environment *env);
static void freeActivityTop(af_Activity *activity);
static void freeAllActivity(af_Activity *activity, af_Environment *env);

/* Activity 相关处理函数 */
static void clearFuncActivity(af_Activity *activity);
static void pushActivity(af_Activity *activity, af_Environment *env);
static void freeMark(af_Activity *activity);
static void newFuncActivity(af_Code *bt, const af_Code *next, bool return_first, af_Environment *env);

/* ActivityTrackBack 创建与释放 */
static af_ActivityTrackBack *makeActivityTrackBack(af_Activity *activity);
static af_ActivityTrackBack *freeActivityTrackBack(af_ActivityTrackBack *atb);
static void freeAllActivityTrackBack(af_ActivityTrackBack *atb);

/* 环境变量 创建与释放 */
static af_EnvVar *makeEnvVar(const char *name);
static af_EnvVar *freeEnvVar(af_EnvVar *var);
static void freeAllEnvVar(af_EnvVar *var);
static void freeEnvVarSpace(af_EnvVarSpace *evs);

/* 顶层消息处理器 创建与释放 */
static af_TopMsgProcess *makeTopMsgProcess(const char *type, DLC_SYMBOL(TopMsgProcessFunc) func);
static af_TopMsgProcess *freeTopMsgProcess(af_TopMsgProcess *mp);
static void freeAllTopMsgProcess(af_TopMsgProcess *mp);

/* 顶层消息处理器 处理函数 */
static af_TopMsgProcess *findTopMsgProcessFunc(const char *type, af_Environment *env);

/* 守护器 创建与释放 */
static af_Guardian *makeGuardian(const char *type, bool always, bool derive, size_t size, DLC_SYMBOL(GuardianFunc) func,
                                 DLC_SYMBOL(GuardianDestruct) destruct);
static af_Guardian *freeGuardian(af_Guardian *gd, af_Environment *env);
static void freeAllGuardian(af_Guardian *gd, af_Environment *env);

/* 守护器 处理函数 */
static af_Guardian *findGuardian(const char *type, af_Environment *env);

/* LiteralData 创建与释放 */
static af_LiteralDataList *makeLiteralDataList(char *data);
static af_LiteralDataList *freeLiteralData_Pri(af_LiteralDataList *ld);

/* LiteralRegex 创建与释放 */
static af_LiteralRegex *makeLiteralRegex(const char *pattern, const char *func, bool in_protect);
static af_LiteralRegex *freeLiteralRegex(af_LiteralRegex *lr);
static void freeAllLiteralRegex(af_LiteralRegex *lr);

/* EnvironmentList 创建与释放 */
static af_EnvironmentList *makeEnvironmentList(af_Environment *env);
static bool freeEnvironmentList(af_EnvironmentList *envl, af_Environment *base);

/* af_ErrorBacktracking 创建与释放 */
static af_ErrorBacktracking *makeErrorBacktracking(FileLine line, FilePath file, const char *note);
static af_ErrorBacktracking *freeErrorBacktracking(af_ErrorBacktracking *ebt);
static void freeAllErrorBacktracking(af_ErrorBacktracking *ebt);

/* af_ErrorBacktracking 相关函数 */
static char *getActivityInfoToBacktracking(af_Activity *activity);
static char *getActivityTrackBackInfoToBacktracking(af_ActivityTrackBack *atb);
static void fprintfNote(FILE *file, const char *note);
static void fprintfNoteStderr(const char *note);
static void fprintfNoteStdout(const char *note);

/* af_GuardianList 创建与释放 */
static af_GuardianList *makeGuardianList(af_Object *obj, af_Object *func, af_Environment *env);
static af_GuardianList *freeGuardianList(af_GuardianList *gl, af_Environment *env);
static void freeAllGuardianList(af_GuardianList *gl, af_Environment *env);

/* 内置顶层消息处理器 */
static void mp_NORMAL(af_Message *msg, bool is_top, af_Environment *env);
static void mp_ERROR(af_Message *msg, bool is_top, af_Environment *env);
static void mp_IMPORT(af_Message *msg, bool is_top, af_Environment *env);
static void mp_NORMALThread(af_Message *msg, bool is_top, af_Environment *env);

/* 变量检查函数 */
static bool isInfixFunc(af_Code *code, af_Environment *env);

char setPrefix(size_t name, char prefix, af_Environment *env) {
    if (name >= PREFIX_SIZE)
        return '-';  // 表示未获取到prefix (NUL在Code中表示无prefix)

    pthread_rwlock_wrlock(&env->esv->lock);
    char *prefix_ = env->prefix->data;
    if (prefix_ == nullptr || strlen(prefix_) < PREFIX_SIZE) {
        pthread_rwlock_unlock(&env->esv->lock);
        return '-';
    }

    switch (name) {
        case E_QUOTE:
            if (prefix == NUL && strchr(E_PREFIX, prefix) == nullptr)
                prefix = '-';
            break;
        case B_EXEC:
        case B_EXEC_FIRST:
            if (prefix == NUL && strchr(B_PREFIX, prefix) == nullptr)
                prefix = '-';
            break;
        default:
            break;
    }
    char old = prefix_[name];
    prefix_[name] = prefix;
    pthread_rwlock_unlock(&env->esv->lock);
    return old;
}

char getPrefix(size_t name, af_Environment *env) {
    if (name >= PREFIX_SIZE)
        return '-';  // 表示未获取到prefix (NUL在Code中表示无prefix)

    pthread_rwlock_rdlock(&env->esv->lock);
    char *prefix = env->prefix->data;
    pthread_rwlock_unlock(&env->esv->lock);
    if (prefix == nullptr || strlen(prefix) < PREFIX_SIZE)
        return '-';
    return prefix[name];
}

af_VarSpace *getProtectVarSpace(af_Environment *env) {
    return env->protect;
}


/*
 * 函数名: getBaseObject
 * 目标: getBaseObjectFromCore的对外接口
 */
af_Object *getBaseObject(const char *name, af_Environment *env) {
    af_Var *var = findVarFromVarSpace(name, nullptr, env->protect);
    if (var != nullptr)
        return findVarNode(var, nullptr, env);
    return nullptr;
}

void setCoreStop(af_Environment *env) {
    pthread_mutex_lock(&env->status_lock);
    if (env->status != core_exit)
        env->status = core_stop;
    pthread_mutex_unlock(&env->status_lock);
}

void setCoreExit(int exit_code, af_Environment *env) {
    pthread_rwlock_wrlock(&env->esv->lock);
    env->exit_code_->num = exit_code;
    pthread_rwlock_unlock(&env->esv->lock);

    setCoreExitNotExitCode(env);
}

void setCoreExitNotExitCode(af_Environment *env) {
    pthread_mutex_lock(&env->status_lock);
    env->status = core_exit;
    pthread_mutex_unlock(&env->status_lock);

    if (!env->is_derive) {  // 若是主线程, 通知所有次线程
        pthread_mutex_lock(&env->thread_lock);
        for (af_EnvironmentList *envl = env->env_list; envl != nullptr; envl = envl->next) {
            pthread_mutex_lock(&envl->env->thread_lock);
            if (envl->env->monitor != nullptr)
                pthread_cond_signal(&envl->env->monitor->cond);
            pthread_mutex_unlock(&envl->env->thread_lock);
        }
        pthread_mutex_unlock(&env->thread_lock);
    }
}

void setCoreNormal(af_Environment *env) {
    pthread_mutex_lock(&env->status_lock);
    if (env->status == core_exit || env->status == core_stop)
        env->status = core_normal;
    pthread_mutex_unlock(&env->status_lock);

    pthread_rwlock_wrlock(&env->esv->lock);
    env->exit_code_->num = 0;
    pthread_rwlock_unlock(&env->esv->lock);
}

static af_Activity *makeActivity(af_Message *msg_up, af_VarList *varlist, af_Object *belong,
                                 af_Environment *env){
    auto activity = calloc(1, af_Activity);
    activity->msg_up = msg_up;
    activity->msg_up_count = 0;
    activity->run_varlist = varlist;
    activity->count_run_varlist = 0;
    activity->belong = belong;
    activity->line = 1;
    activity->gc_lock = &env->base->gc_factory->mutex;
    return activity;
}

static af_Activity *makeFuncActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                     af_VarList *out_varlist, af_Object *belong, af_Object *func,
                                     af_Environment *env){
    af_Activity *activity = makeActivity(msg_up, out_varlist, belong, env);

    activity->type = act_func;
    activity->status = act_func_get;
    activity->func = func;

    activity->out_varlist = out_varlist;
    activity->func_varlist = nullptr;

    setActivityBtTop(bt_top, activity);  // 非NORMAL期间, bt_top被设定
    setActivityBtStart(bt_start, activity);

    activity->return_first = return_first;
    return activity;
}

static af_Activity *makeTopActivity(af_Code *bt_top, af_Code *bt_start, af_VarSpace *protect, af_Object *belong,
                                    af_Environment *env){
    af_Activity *activity = makeActivity(nullptr, nullptr, belong, env);

    activity->type = act_top;
    activity->status = act_func_normal;

    activity->count_run_varlist = 2;
    activity->run_varlist = pushVarList(protect, nullptr);
    activity->run_varlist = pushVarList(getObjectVarSpace(belong), activity->run_varlist);

    setActivityBtTop(nullptr, activity);  // top-activity直接就在normal, bt_top将不被设定
    setActivityBtStart(bt_start, activity);

    return activity;
}

static af_Activity *makeTopImportActivity(af_Code *bt_top, af_Code *bt_start, af_VarSpace *protect, af_Object *belong,
                                          const char *mark,af_Environment *env){
    af_Activity *activity = makeTopActivity(bt_top, bt_start, protect, belong, env);
    activity->type = act_top_import;
    if (mark != nullptr)
        activity->import_mark = strCopy(mark);
    return activity;
}

static af_Activity *makeGuardianActivity(af_GuardianList *gl, af_GuardianList **pgl, af_Environment *env) {
    af_Activity *activity = makeActivity(nullptr, nullptr, env->global, env);
    activity->type = act_guardian;

    activity->run_varlist = pushProtectVarList(nullptr, env);
    activity->count_run_varlist = 1;

    activity->file = strCopy("guardian.aun.sys");
    activity->line = 1;
    activity->gl = gl;
    activity->pgl = pgl;
    activity->gl_next = gl;
    activity->is_guard = true;
    return activity;
}

static af_Activity *freeActivity(af_Activity *activity, af_Environment *env){
    pthread_mutex_lock(activity->gc_lock);
    af_Activity *prev = activity->prev;

    freeAllMessage(activity->msg_down);  // msg转移后需要将对应成员设置为nullptr
    freeMessageCount(activity->msg_up_count, activity->msg_up);

    free(activity->file);
    freeVarSpaceListCount(activity->count_run_varlist, activity->run_varlist);

    if (activity->type == act_guardian) {
        if (activity->gl != nullptr)
            freeAllGuardianList(activity->gl, env);
    } else {
        freeVarSpaceListCount(activity->count_out_varlist, activity->out_varlist);
        freeVarSpaceListCount(activity->count_macro_varlist, activity->macro_varlist);

        freeAllArgCodeList(activity->acl_start, env);
        if (activity->fi != nullptr)
            freeFuncInfo(activity->fi);
        freeAllLiteralData(activity->ld);

        freeAllActivityTrackBack(activity->tb);
        free(activity->import_mark);
    }

    pthread_mutex_unlock(activity->gc_lock);
    free(activity);
    return prev;
}

static void freeActivityTop(af_Activity *activity) {
    freeAllMessage(activity->msg_down);  // msg转移后需要将对应成员设置为nullptr
    freeMessageCount(activity->msg_up_count, activity->msg_up);

    setActivityBtTop(nullptr, activity);
    setActivityBtStart(nullptr, activity);
    free(activity->file);
}

static void freeAllActivity(af_Activity *activity, af_Environment *env){
    while (activity != nullptr)
        activity = freeActivity(activity, env);
}

static void pushActivity(af_Activity *activity, af_Environment *env) {
    if (env->activity->is_guard)
        activity->is_guard = true;

    activity->prev = env->activity;
    env->activity = activity;
}

/*
 * 函数名: clearActivity
 * 目标: 尾调用优化时用于清理Activity.
 * file和line都遗留
 * 清空 varlist (除 run_varlist)
 */
static void clearFuncActivity(af_Activity *activity) {
    /* acl在runArgList之后就被释放了 */
    /* acl在FuncBody暂时不释放 */

    pthread_mutex_lock(activity->gc_lock);
    activity->out_varlist = activity->run_varlist;
    activity->count_out_varlist = activity->count_run_varlist;
    activity->count_run_varlist = 0;

    activity->macro_varlist = nullptr;
    activity->count_macro_varlist = 0;

    activity->func_varlist = nullptr;

    /* mark在setFuncActivityToNormal被清理*/
    /* 只有FuncBody执行到最后一个(意味着Mark被清理)后才会有尾调用优化 */
    activity->mark = nullptr;
    activity->func = nullptr;
    pthread_mutex_unlock(activity->gc_lock);

    setActivityBtTop(nullptr, activity);
    setActivityBtStart(nullptr, activity);

    /* acl_start 在 setFuncActivityAddVar 时被释放 */
    activity->acl_start = nullptr;
    activity->acl_done = nullptr;

    /* activity->fi 暂时不清理, 直到setFuncActivityAddVar时才清理 */
    activity->body_next = nullptr;
}

/*
 * 函数名: af_ActivityTrackBack
 * 目标: 把 activity 上的内容转移到新的 af_ActivityTrackBack 上
 */
static af_ActivityTrackBack *makeActivityTrackBack(af_Activity *activity) {
    auto atb = calloc(1, af_ActivityTrackBack);
#define EXCHANGE(name) (atb->name = activity->name)
    EXCHANGE(file);
    atb->file = strCopy(activity->file);
    EXCHANGE(status);
    EXCHANGE(line);
    EXCHANGE(return_first);
    EXCHANGE(run_in_func);
    EXCHANGE(is_macro_call);
    EXCHANGE(is_gc_call);
    EXCHANGE(is_literal);
    EXCHANGE(is_obj_func);
    EXCHANGE(is_execution);
    EXCHANGE(optimization);
#undef EXCHANGE
    return atb;
}

static af_ActivityTrackBack *freeActivityTrackBack(af_ActivityTrackBack *atb) {
    af_ActivityTrackBack *next = atb->next;
    free(atb->file);
    free(atb);
    return next;
}

static void freeAllActivityTrackBack(af_ActivityTrackBack *atb) {
    while (atb != nullptr)
        atb = freeActivityTrackBack(atb);
}

/*
 * 函数名: tailCallActivity
 * 目标: 记录ActivityTrackBack然后清除Activity
 */
static void tailCallActivity(af_Object *func, af_Activity *activity) {
    af_ActivityTrackBack *atb = makeActivityTrackBack(activity);
    atb->next = activity->tb;
    clearFuncActivity(activity);
    activity->tb = atb;

    pthread_mutex_lock(activity->gc_lock);
    activity->func = func;
    pthread_mutex_unlock(activity->gc_lock);
}

/*
 * 函数名: setActivityBtTop
 * 目标: 设置activity的bt_top, 并且设置行号
 * bt_start和bt_next会被设置为nullptr
 */
void setActivityBtTop(af_Code *bt_top, af_Activity *activity) {
    activity->bt_top = bt_top;
    activity->bt_start = nullptr;
    activity->bt_next = nullptr;
    if (bt_top != nullptr) {
        activity->line = bt_top->line;
        if (bt_top->path != nullptr) {
            free(activity->file);
            activity->file = strCopy(bt_top->path);
        }
    } else
        activity->line = 1;
}

/*
 * 函数名: setActivityBtStart
 * 目标: 设置activity的bt_start, 并且设置行号
 * bt_next会被设置为nullptr
 */
void setActivityBtStart(af_Code *bt_start, af_Activity *activity) {
    activity->bt_start = bt_start;
    activity->bt_next = bt_start;
    if (bt_start != nullptr) {
        activity->line = bt_start->line;
        if (bt_start->path != nullptr) {
            free(activity->file);
            activity->file = strCopy(bt_start->path);
        }
    } else
        activity->line = 1;
}

/*
 * 函数名: setActivityBtNext
 * 目标: 设置activity的bt_next, 并且设置行号
 */
void setActivityBtNext(af_Code *bt_next, af_Activity *activity) {
    activity->bt_next = bt_next;
    if (bt_next != nullptr) {
        activity->line = bt_next->line;
        if (bt_next->path != nullptr) {
            free(activity->file);
            activity->file = strCopy(bt_next->path);
        }
    } else
        activity->line = 1;
}

/*
 * 函数名: makeLiteralDataList
 * 目标: 生成一个 af_LiteralDataList
 * 注意: char *data 要求传入一个已经被复制的data值
 * makeLiteralDataList是内部函数, 属于可控函数, 因此data在函数内部不再复制
 */
static af_LiteralDataList *makeLiteralDataList(char *data) {
    auto ld = calloc(1, af_LiteralDataList);
    ld->literal_data = data;
    return ld;
}

static af_LiteralDataList *freeLiteralData_Pri(af_LiteralDataList *ld) {
    af_LiteralDataList *next = ld->next;
    free(ld->literal_data);
    free(ld);
    return next;
}

void freeAllLiteralData(af_LiteralDataList *ld) {
    while (ld != nullptr)
        ld = freeLiteralData_Pri(ld);
}

void pushLiteralData(char *data, af_Environment *env) {
    af_LiteralDataList *ld = makeLiteralDataList(data);
    ld->next = env->activity->ld;
    env->activity->ld = ld;
}

af_Message *makeMessage(const char *type, size_t size) {
    auto msg = calloc(1, af_Message);
    msg->type = strCopy(type);
    if (size != 0)
        msg->msg = calloc_size(1, size);
    msg->size = size;
    return msg;
}

af_Message *freeMessage(af_Message *msg) {
    af_Message *next = msg->next;
    free(msg->type);
    free(msg->msg);
    free(msg);
    return next;
}

void freeAllMessage(af_Message *msg) {
    while (msg != nullptr)
        msg = freeMessage(msg);
}

bool freeMessageCount(size_t count, af_Message *msg) {
    for (size_t i = count; i > 0; i--) {
        if (msg == nullptr)  // 发生了错误
            return false;
        msg = freeMessage(msg);
    }
    return true;
}

void pushMessageUp(af_Message *msg, af_Environment *env) {
    msg->next = env->activity->msg_up;
    env->activity->msg_up = msg;
    env->activity->msg_up_count++;
}

void pushMessageDown(af_Message *msg, af_Environment *env) {
    msg->next = env->activity->msg_down;
    env->activity->msg_down = msg;
}

void *popMessageUpData(const char *type, af_Environment *env) {
    for (af_Message **pmsg = &env->activity->msg_up; *pmsg != nullptr; pmsg = &((*pmsg)->next)) {
        if (EQ_STR((*pmsg)->type, type))
            return (*pmsg)->msg;  // msg_up是只读的
    }
    return nullptr;
}

af_Message *popMessageUp(af_Environment *env) {
    if (env->activity->msg_up_count == 0 || env->activity->msg_up == nullptr)
        return nullptr;

    af_Message *msg = env->activity->msg_up;
    env->activity->msg_up = msg->next;
    msg->next = nullptr;
    env->activity->msg_up_count--;
    return msg;
}

/*
 * 函数名: getMessageData
 * 目标: 获取`msg`的数据, 对外API
 */
void *getMessageData(af_Message *msg) {
    return msg->msg;
}

af_Message *popMessageDown(const char *type, af_Environment *env) {
    for (af_Message **pmsg = &env->activity->msg_down; *pmsg != nullptr; pmsg = &((*pmsg)->next)) {
        if (EQ_STR((*pmsg)->type, type)) {
            af_Message *msg = *pmsg;
            *pmsg = msg->next;
            msg->next = nullptr;
            return msg;
        }
    }
    return nullptr;
}

af_Message *getFirstMessage(af_Environment *env) {
    af_Message *msg = env->activity->msg_down;
    env->activity->msg_down = msg->next;
    msg->next = nullptr;
    return msg;
}

void connectMessage(af_Message **base, af_Message *msg) {
    while (*base != nullptr)
        base = &((*base)->next);
    *base = msg;
}

/**
 * 生成类型为NORMAL的Message
 * 注意: obj 必须提前添加 gc_addReference
 * @param obj
 * @param env
 * @return
 */
af_Message *makeNORMALMessage(af_Object *obj, af_Environment *env){
    af_Message *msg = makeMessage("NORMAL", sizeof(af_Object *));
    *(af_Object **)msg->msg = obj;
    return msg;
}

af_Message *makeERRORMessage(const char *type, const char *error, af_Environment *env) {
    char *info = nullptr;
    af_ErrorInfo *ei = nullptr;

    for (af_Activity *activity = env->activity; activity != nullptr; activity = activity->prev) {
        info = getActivityInfoToBacktracking(activity);
        if (ei == nullptr)
            ei = makeErrorInfo(type, error, info, env->activity->line, env->activity->file);
        else
            pushErrorBacktracking(activity->line, activity->file, info, ei);
        free(info);

        for (af_ActivityTrackBack *atb = activity->tb; atb != nullptr; atb = atb->next) {
            info = getActivityTrackBackInfoToBacktracking(atb);
            pushErrorBacktracking(atb->line, atb->file, info, ei);
            free(info);
        }
    }

    af_Message *msg = makeMessage("ERROR", sizeof(af_ErrorInfo *));
    *(af_ErrorInfo **)msg->msg = ei;
    return msg;
}

af_Message *makeERRORMessageFormat(const char *type, af_Environment *env, const char *format, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, 1024, format, ap);
    va_end(ap);
    return makeERRORMessage(type, buf, env);;
}

/**
 * 生成类型为IMPORT的Message
 * 注意: obj 必须提前添加 gc_addReference
 * @param mark
 * @param obj
 * @param env
 * @return
 */
af_Message *makeIMPORTMessage(const char *mark, af_Object *obj, af_Environment *env){
    af_Message *msg = makeMessage("IMPORT", sizeof(af_ImportInfo *));
    *(af_ImportInfo **)msg->msg = makeImportInfo(mark, obj, env);
    return msg;
}

static af_EnvVar *makeEnvVar(const char *name) {
    auto var = calloc(1, af_EnvVar);
    var->name = strCopy(name);
    return var;
}

static af_EnvVar *freeEnvVar(af_EnvVar *var) {
    af_EnvVar *next = var->next;
    free(var->data);
    free(var->name);
    free(var);
    return next;
}

static void freeAllEnvVar(af_EnvVar *var) {
    while (var != nullptr)
        var = freeEnvVar(var);
}

static af_EnvVarSpace *makeEnvVarSpace() {
    auto evs = calloc(1, af_EnvVarSpace);
    pthread_rwlock_init(&evs->lock, nullptr);
    return evs;
}

static void freeEnvVarSpace(af_EnvVarSpace *evs) {
    for (int i = 0; i < ENV_VAR_HASH_SIZE; i++)
        freeAllEnvVar(evs->var[i]);
    pthread_rwlock_destroy(&evs->lock);
    free(evs);
}

af_EnvVar *setEnvVarData_(const char *name, const char *data, af_Environment *env) {
    time33_t index = time33(name) % ENV_VAR_HASH_SIZE;
    pthread_rwlock_wrlock(&env->esv->lock);

    af_EnvVar **pvar = &env->esv->var[index];
    env->esv->count++;
    for(NULL; *pvar != nullptr; pvar = &((*pvar)->next)) {
        if (EQ_STR((*pvar)->name, name)) {
            free((*pvar)->data);
            (*pvar)->data = strCopy(data);
            pthread_rwlock_unlock(&env->esv->lock);
            return *pvar;
        }
    }

    *pvar = makeEnvVar(name);
    (*pvar)->data = strCopy(data);
    pthread_rwlock_unlock(&env->esv->lock);
    return *pvar;
}

af_EnvVar *setEnvVarNumber_(const char *name, int32_t data, af_Environment *env) {
    time33_t index = time33(name) % ENV_VAR_HASH_SIZE;
    pthread_rwlock_wrlock(&env->esv->lock);

    af_EnvVar **pvar = &env->esv->var[index];
    env->esv->count++;
    for(NULL; *pvar != nullptr; pvar = &((*pvar)->next)) {
        if (EQ_STR((*pvar)->name, name)) {
            free((*pvar)->data);
            (*pvar)->num = data;
            pthread_rwlock_unlock(&env->esv->lock);
            return *pvar;
        }
    }

    *pvar = makeEnvVar(name);
    (*pvar)->num = data;
    pthread_rwlock_unlock(&env->esv->lock);
    return *pvar;
}

void setEnvVarData(const char *name, const char *data, af_Environment *env) {
    setEnvVarData_(name, data, env);
}

void setEnvVarNumber(const char *name, int32_t data, af_Environment *env) {
    setEnvVarNumber_(name, data, env);
}

char *findEnvVarData(const char *name, af_Environment *env) {
    time33_t index = time33(name) % ENV_VAR_HASH_SIZE;
    pthread_rwlock_rdlock(&env->esv->lock);

    af_EnvVar **pvar = &env->esv->var[index];
    for(NULL; *pvar != nullptr; pvar = &((*pvar)->next)) {
        if (EQ_STR((*pvar)->name, name)) {
            char *data = (*pvar)->data;
            pthread_rwlock_unlock(&env->esv->lock);
            return data;
        }
    }

    pthread_rwlock_unlock(&env->esv->lock);
    return nullptr;
}

int32_t *findEnvVarNumber(const char *name, af_Environment *env) {
    time33_t index = time33(name) % ENV_VAR_HASH_SIZE;
    pthread_rwlock_rdlock(&env->esv->lock);

    af_EnvVar **pvar = &env->esv->var[index];
    for(NULL; *pvar != nullptr; pvar = &((*pvar)->next)) {
        if (EQ_STR((*pvar)->name, name)) {
            int32_t *data = &(*pvar)->num;  // 返回指针, nullptr表示没找到
            pthread_rwlock_unlock(&env->esv->lock);
            return data;
        }
    }

    pthread_rwlock_unlock(&env->esv->lock);
    return nullptr;
}

static void mp_NORMAL(af_Message *msg, bool is_top, af_Environment *env) {
    if (msg->msg == nullptr || *(af_Object **)msg->msg == nullptr) {
        writeErrorLog(aFunCoreLogger, "NORMAL msg: %p error", msg->msg);
        return;
    }
    if (is_top)
        writeDebugLog(aFunCoreLogger, "NORMAL Point: %p", *(af_Object **)msg->msg);
    gc_delObjectReference(*(af_Object **)msg->msg, env);
    *(af_Object **)msg->msg = nullptr;
}

static void mp_NORMALThread(af_Message *msg, bool is_top, af_Environment *env) {
    if (msg->msg == nullptr || *(af_Object **)msg->msg == nullptr) {
        writeErrorLog(aFunCoreLogger, "Thread-NORMAL msg: %p error", msg->msg);
        return;
    }

    if (is_top)
        writeDebugLog(aFunCoreLogger, "Thread-NORMAL Point: %p", *(af_Object **)msg->msg);

    pthread_mutex_lock(&env->thread_lock);
    env->result = *(af_Object **)msg->msg;
    gc_delObjectReference(env->result, env);
    *(af_Object **)msg->msg = nullptr;
    pthread_mutex_unlock(&env->thread_lock);
}

static void mp_ERROR(af_Message *msg, bool is_top, af_Environment *env) {
    if (msg->msg == nullptr || *(af_ErrorInfo **)msg->msg == nullptr) {
        writeErrorLog(aFunCoreLogger, "ERROR msg: %p error", msg->msg);
        return;
    }
    if (is_top) {
        if (getErrorStd(env) == 0)
            fprintfErrorInfoStdout(*(af_ErrorInfo **) msg->msg);
        else
            fprintfErrorInfoStderr(*(af_ErrorInfo **) msg->msg);
    }
    freeErrorInfo(*(af_ErrorInfo **) msg->msg, env);
}

static void mp_IMPORT(af_Message *msg, bool is_top, af_Environment *env) {
    if (msg->msg == nullptr || *(af_ImportInfo **)msg->msg == nullptr) {
        writeErrorLog(aFunCoreLogger, "IMPORT msg: %p error", msg->msg);
        return;
    }
    af_ImportInfo *ii = *(af_ImportInfo **)msg->msg;
    if (ii->obj == nullptr) {
        writeErrorLog(aFunCoreLogger, "IMPORT msg: %p do not get obj", msg->msg);
        return;
    }

    if (ii->mark != nullptr) {
        makeVarToProtectVarSpace(ii->mark, 3, 3, 3, ii->obj, env);
        writeDebugLog(aFunCoreLogger, "IMPORT point: [%s] %p", ii->mark, ii->obj);
    } else
        writeDebugLog(aFunCoreLogger, "IMPORT point: <no-name> %p", ii->obj);
    freeImportInfo(ii, env);
}

af_Environment *makeEnvironment(enum GcRunTime grt) {
    auto env = calloc(1, af_Environment);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&env->in_run, &attr);  // 检测锁
    pthread_mutexattr_destroy(&attr);

    pthread_mutex_init(&env->thread_lock, nullptr);
    pthread_mutex_init(&env->status_lock, nullptr);

    env->is_derive = false;
    env->base = env;

    env->gc_factory = makegGcFactory();
    env->esv = makeEnvVarSpace();

    /* 设置默认prefix */
    char prefix[PREFIX_SIZE + 1] = "";
    prefix[E_QUOTE] = '\'';
    prefix[B_EXEC] = '\'';
    prefix[B_EXEC_FIRST] = '$';
    env->prefix = setEnvVarData_(ev_sys_prefix, prefix, env);
    env->gc_runtime = setEnvVarNumber_(ev_grt, grt, env);
    env->gc_max = setEnvVarNumber_(ev_gcmax, DEFAULT_GC_COUNT_MAX, env);
    env->gc_count = setEnvVarNumber_(ev_gccount, 0, env);
    env->exit_code_ = setEnvVarNumber_(ev_exit_code, 0, env);
    env->argc = setEnvVarNumber_(ev_argc, 0, env);
    env->error_std = setEnvVarNumber_(ev_error_std, 0, env);

    /* 创建保护空间 */
    env->protect = makeVarSpace(nullptr, 3, 3, 3, env);

    /* 生成global对象 */
    env->global = makeGlobalObject(env);

    /* 设置NORMAL顶级处理器 */
    DLC_SYMBOL(TopMsgProcessFunc) func1 = MAKE_SYMBOL(mp_NORMAL, TopMsgProcessFunc);
    addTopMsgProcess("NORMAL", func1, env);
    FREE_SYMBOL(func1);

    DLC_SYMBOL(TopMsgProcessFunc) func2 = MAKE_SYMBOL(mp_ERROR, TopMsgProcessFunc);
    addTopMsgProcess("ERROR", func2, env);
    FREE_SYMBOL(func2);

    DLC_SYMBOL(TopMsgProcessFunc) func3 = MAKE_SYMBOL(mp_IMPORT, TopMsgProcessFunc);
    addTopMsgProcess("IMPORT", func3, env);
    FREE_SYMBOL(func3);

    env->status = core_init;
    env->activity = makeTopActivity(nullptr, nullptr, env->protect, env->global, env);

    makeVarToProtectVarSpace("global", 3, 3, 3, env->global, env);

    gc_delVarSpaceReference(env->protect, env);
    gc_delObjectReference(env->global, env);
    return env;
}

af_Environment *deriveEnvironment(bool derive_tmp, bool derive_guardian, bool derive_lr, bool enable,
                                  af_Environment *base) {
    auto env = calloc(1, af_Environment);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&env->in_run, &attr);  // 检测锁
    pthread_mutexattr_destroy(&attr);

    pthread_mutex_init(&env->thread_lock, nullptr);
    pthread_mutex_init(&env->status_lock, nullptr);

    env->is_derive = true;
    env->base = base->base;
    pushEnvironmentList(env, base);

#define D(v) env->v = (env->base)->v
    D(gc_factory);
    D(esv);
    D(prefix);
    D(gc_runtime);
    D(gc_max);
    D(gc_count);
    D(exit_code_);
    D(argc);
    D(error_std);
    D(protect);
    D(global);
#undef D

    if (derive_tmp) {
        for (af_TopMsgProcess *tmp = base->process; tmp != nullptr; tmp = tmp->next)
            addTopMsgProcess(tmp->type, tmp->func, env);
    }

    DLC_SYMBOL(TopMsgProcessFunc) func1 = MAKE_SYMBOL(mp_NORMALThread, TopMsgProcessFunc);
    af_TopMsgProcess *tmp = findTopMsgProcessFunc("NORMAL", env);
    if (tmp == nullptr) {
        addTopMsgProcess("NORMAL", func1, env);
        FREE_SYMBOL(func1);
    } else {
        FREE_SYMBOL(tmp->func);
        tmp->func = func1;
    }

    if (derive_guardian) {
        for (af_Guardian *gu = base->guardian; gu != nullptr; gu = gu->next) {
            if (gu->derive)
                addGuardian(gu->type, gu->always, true, gu->size, gu->func, gu->destruct, nullptr, env);
        }
    }

    if (derive_lr) {
        for (af_LiteralRegex *lr = base->lr; lr != nullptr; lr = lr->next)
            pushLiteralRegex(lr->pattern, lr->func, lr->in_protect, env);
    }

    if (enable)
        env->status = core_normal;
    else
        env->status = core_init;

    env->activity = makeTopActivity(nullptr, nullptr, env->protect, env->global, env);
    return env;
}

void enableEnvironment(af_Environment *env) {
    env->status = core_normal;
}

bool freeEnvironment(af_Environment *env) {
    bool res = true;
    if (!env->is_derive && getEnviromentSonCount(env) != 0)
        return false;

    if (!env->is_derive && env->status != core_creat)
        res = iterDestruct(10, env);

    freeAllActivity(env->activity, env);
    freeAllTopMsgProcess(env->process);
    freeAllGuardian(env->guardian, env);
    freeAllLiteralRegex(env->lr);

    if (!env->is_derive) {
        freeEnvVarSpace(env->esv);

        gc_freeAllValueData(env);  // 先释放ObjectData的void *data
        printGCByCore(env);
        gc_freeAllValue(env);  // 再完全释放Object
        freeGcFactory(env->gc_factory);
    } else
        freeEnvironmentListByEnv(env, env->base);

    pthread_mutex_destroy(&env->in_run);
    pthread_mutex_destroy(&env->thread_lock);
    if (!res)
        writeErrorLog(aFunCoreLogger, "Run iterDestruct error.");
    free(env);
    return true;
}

static af_TopMsgProcess *makeTopMsgProcess(const char *type, DLC_SYMBOL(TopMsgProcessFunc) func) {
    auto mp = calloc(1, af_TopMsgProcess);
    mp->type = strCopy(type);
    mp->func = COPY_SYMBOL(func, TopMsgProcessFunc);
    return mp;
}

static af_TopMsgProcess *freeTopMsgProcess(af_TopMsgProcess *mp) {
    af_TopMsgProcess *next = mp->next;
    free(mp->type);
    FREE_SYMBOL(mp->func);
    free(mp);
    return next;
}

static void freeAllTopMsgProcess(af_TopMsgProcess *mp) {
    while (mp != nullptr)
        mp = freeTopMsgProcess(mp);
}

static af_TopMsgProcess *findTopMsgProcessFunc(const char *type, af_Environment *env) {
    auto mp = env->process;
    for(NULL; mp != nullptr; mp = mp->next) {
        if (EQ_STR(type, mp->type))
            return mp;
    }
    return nullptr;
}

bool addTopMsgProcess(const char *type, DLC_SYMBOL(TopMsgProcessFunc) func, af_Environment *env) {
    auto mp = findTopMsgProcessFunc(type, env);
    if (mp != nullptr)
        return false;

    mp = makeTopMsgProcess(type, func);
    mp->next = env->process;
    env->process = mp;
    return true;
}

static af_Guardian *makeGuardian(const char *type, bool always, bool derive, size_t size, DLC_SYMBOL(GuardianFunc) func,
                                 DLC_SYMBOL(GuardianDestruct) destruct){
    auto gd = calloc(1, af_Guardian);
    gd->type = strCopy(type);
    gd->always = always;
    gd->derive = derive;

    if (size != 0) {
        gd->data = calloc_size(1, size);
        gd->size = size;
    }

    gd->func = COPY_SYMBOL(func, GuardianFunc);

    if (destruct != nullptr)
        gd->destruct = COPY_SYMBOL(destruct, GuardianDestruct);
    return gd;
}

static af_Guardian *freeGuardian(af_Guardian *gd, af_Environment *env) {
    af_Guardian *next = gd->next;
    if (gd->data != nullptr && gd->destruct != nullptr)
        GET_SYMBOL(gd->destruct)(gd->type, gd->data, env);
    free(gd->data);
    free(gd->type);
    FREE_SYMBOL(gd->func);
    FREE_SYMBOL(gd->destruct);
    free(gd);
    return next;
}

static void freeAllGuardian(af_Guardian *gd, af_Environment *env) {
    while (gd != nullptr)
        gd = freeGuardian(gd, env);
}

static af_Guardian *findGuardian(const char *type, af_Environment *env) {
    af_Guardian *gd = env->guardian;
    for(NULL; gd != nullptr; gd = gd->next) {
        if (EQ_STR(type, gd->type))
            return gd;
    }
    return nullptr;
}

bool addGuardian(const char *type, bool always, bool derive, size_t size, DLC_SYMBOL(GuardianFunc) func,
                 DLC_SYMBOL(GuardianDestruct) destruct, void **pdata, af_Environment *env){
    af_Guardian *gd = findGuardian(type, env);
    if (gd != nullptr)
        return false;

    gd = makeGuardian(type, always, derive, size, func, destruct);
    gd->next = env->guardian;
    env->guardian = gd;
    if (pdata != nullptr)
        *pdata = gd->data;
    return true;
}

bool popGuardian(const char *type, af_Environment *env) {
    af_Guardian **gd = &env->guardian;
    for(NULL; *gd != nullptr; gd = &((*gd)->next)) {
        if (EQ_STR(type, (*gd)->type)) {
            *gd = freeGuardian(*gd, env);
            return true;
        }
    }

    return false;
}

static void newFuncActivity(af_Code *bt, const af_Code *next, bool return_first, af_Environment *env){
    if (next == nullptr && env->activity->body_next == nullptr &&
        env->activity->type == act_func && env->activity->macro_varlist == nullptr) {
        writeDebugLog(aFunCoreLogger, "Tail call optimization");
        tailCallActivity(getActivityFunc(env), env->activity);
        setActivityBtTop(bt, env->activity);
        env->activity->optimization = true;
        if (!env->activity->return_first)  // 若原本就有设置 return_first 则没有在设置的必要了, 因为该执行不会被返回
            env->activity->return_first = return_first;
    } else {
        af_Activity *activity = makeFuncActivity(bt, nullptr, return_first, env->activity->msg_up,
                                                 env->activity->run_varlist, env->activity->belong,
                                                 getActivityFunc(env), env);
        pushActivity(activity, env);
    }
}

/*
 * 函数名: isInfixFunc
 * 目标: 检查是否中缀调用函数
 */
static bool isInfixFunc(af_Code *code, af_Environment *env) {
    if (code == nullptr || code->type != code_element || code->prefix == getPrefix(E_QUOTE, env))  // 检查是否element, 且无引用前缀
        return false;

    if (checkLiteralCode(code->element.data, nullptr, nullptr, env))  // 检查是否字面量
        return false;

    writeDebugLog(aFunCoreLogger, "isInfixFunc: %s\n", code->element.data);
    af_Var *var = findVarFromVarList(code->element.data, env->activity->belong, env->activity->run_varlist);
    if (var == nullptr)
        return false;

    af_Object *obj = findVarNode(var, nullptr, env);
    auto func = (obj_isInfixFunc *)findAPI("obj_isInfixFunc", getObjectAPI(obj));
    if (func == nullptr) {
        gc_delObjectReference(obj, env);
        return false;
    }

    bool res = func(getObjectID(obj), obj);
    gc_delObjectReference(obj, env);
    return res;
}

bool pushExecutionActivity(af_Code *bt, bool return_first, af_Environment *env) {
    af_Code *next;
    next = getCodeNext(bt);
    writeDebugLog(aFunCoreLogger, "Run execution");

    if (bt->type != code_block || bt->block.is_empty) {
        pushMessageDown(makeERRORMessage(SYNTAX_ERROR, NOT_CODE_INFO, env), env);
        return false;
    }

    setActivityBtNext(next, env->activity);

    newFuncActivity(bt, next, return_first, env);
    setActivityBtStart(bt->next, env->activity);

    env->activity->status = act_func_normal;
    env->activity->is_execution = true;
    return true;
}

bool pushFuncActivity(af_Code *bt, af_Environment *env) {
    af_Code *next;
    af_Code *func;
    af_Object *parentheses_call = env->activity->parentheses_call;

    pthread_mutex_lock(env->activity->gc_lock);
    env->activity->parentheses_call = nullptr;
    pthread_mutex_unlock(env->activity->gc_lock);

    writeDebugLog(aFunCoreLogger, "Run func");
    next = getCodeNext(bt);
    switch (bt->block.type) {
        case curly:
            if (bt->block.is_empty) {
                pushMessageDown(makeERRORMessage(CALL_ERROR, CURLY_FUNC_BODY_INFO, env), env);
                return false;
            }
            func = bt->next;
            break;
        case brackets:
            func = nullptr;
            for (af_Code *code = bt->next; code != nullptr; code = getCodeNext(code)) {
                if (isInfixFunc(code, env)) {
                    func = code;
                    break;
                }
            }
            if (func == nullptr) {
                pushMessageDown(makeERRORMessage(CALL_ERROR, BRACKETS_FUNC_BODY_INFO, env), env);
                return false;
            }
            break;
        case parentheses:
            func = nullptr;  // 小括号则不在需要匹配
            break;
        default:
            break;
    }

    setActivityBtNext(next, env->activity);  // 设置当前Activity的bt_next

    newFuncActivity(bt, next, false, env);  // 添加新的Activity或尾调用优化
    setActivityBtStart(func, env->activity);

    env->activity->call_type = env->activity->bt_top->block.type;  // newActivity时会设置bt_top
    env->activity->status = act_func_get;
    if (env->activity->call_type == parentheses) { // 对于类前缀调用, 已经获得func的实际值了
        if (parentheses_call == nullptr) {
            pushMessageDown(makeERRORMessage(CALL_ERROR, PARENTHESES_FUNC_BODY_INFO, env), env);
            return false;
        }
        return setFuncActivityToArg(parentheses_call, env);
    }
    return true;
}

bool pushLiteralActivity(af_Code *bt, const char *data, af_Object *func, af_Environment *env) {
    setActivityBtNext(bt->next, env->activity);

    writeDebugLog(aFunCoreLogger, "Run literal");
    newFuncActivity(bt, bt->next, false, env);
    env->activity->is_literal = true;
    pushLiteralData(strCopy(data), env);  // FuncBody的释放导致code和literal_data释放, 所以要复制
    return setFuncActivityToArg(func, env);
}

bool pushVariableActivity(af_Code *bt, af_Object *func, af_Environment *env) {
    setActivityBtNext(bt->next, env->activity);

    writeDebugLog(aFunCoreLogger, "Run variable");
    newFuncActivity(bt, bt->next, false, env);
    env->activity->is_obj_func = true;
    return setFuncActivityToArg(func, env);
}

bool pushMacroFuncActivity(af_Object *func, af_Environment *env) {
    /* Macro是隐式调用, bt不移动 */
    /* 沿用activity */
    af_VarList *macro_varlist = env->activity->macro_varlist;
    ActivityCount count = env->activity->count_macro_varlist;
    env->activity->count_macro_varlist = 0;

    pthread_mutex_lock(env->activity->gc_lock);
    af_VarList *tmp = env->activity->run_varlist;
    env->activity->run_varlist = nullptr;
    pthread_mutex_unlock(env->activity->gc_lock);

    writeDebugLog(aFunCoreLogger, "Run macro");
    if (!freeVarSpaceListCount(env->activity->count_run_varlist, tmp)) { // 释放外部变量空间
        env->activity->count_run_varlist = 0;
        pushMessageDown(makeERRORMessage(RUN_ERROR, FREE_VARSPACE_INFO, env), env);
        return false;
    }

    env->activity->count_run_varlist = 0;
    tailCallActivity(func, env->activity);  /* 隐式调用不设置 bt_top */

    /* tailCallActivity 会清除 out_varlist 的设定 */
    pthread_mutex_lock(env->activity->gc_lock);
    env->activity->out_varlist = macro_varlist;
    env->activity->count_out_varlist = count;
    env->activity->is_macro_call = true;
    pthread_mutex_unlock(env->activity->gc_lock);
    return setFuncActivityToArg(func, env);
}

void pushGuardianActivity(af_GuardianList *gl, af_GuardianList **pgl, af_Environment *env) {
    for (af_Activity *tmp = env->activity; tmp != nullptr; tmp = tmp->prev) {
        if (tmp->type == act_guardian) {
            *(tmp->pgl) = gl;
            tmp->pgl = pgl;
            if (tmp->gl_next == nullptr)  // 原dl_next已经运行到末端
                tmp->gl_next = gl;
            return;
        }
    }

    /* gc Activity 可能创建为顶层 activity, 故信息不能继承上一级(可能没有上一级) */
    af_Activity *activity = makeGuardianActivity(gl, pgl, env);
    pushActivity(activity, env);
}

bool pushImportActivity(af_Code *bt, af_Object **obj, const char *mark, af_Environment *env) {
    af_Object *tmp = nullptr;
    if (obj != nullptr)
        tmp = *obj;

    if (tmp == nullptr)
        tmp = makeGlobalObject(env);

    af_Activity *activity = makeTopImportActivity(bt, bt, env->protect, tmp, mark, env);
    pushActivity(activity, env);

    if (obj != nullptr)
        *obj = tmp;
    else
        gc_delObjectReference(tmp, env);

    return true;
}

bool pushGuadianFuncActivity(af_GuardianList *gl, af_Environment *env) {
    env->activity->gl_next = gl->next;

    af_Object *belong = gl->obj != nullptr ? gl->obj : env->global;
    /* 隐式调用不设置 bt_top */
    af_Activity *activity = makeFuncActivity(nullptr, nullptr, false, env->activity->msg_up,
                                             env->activity->run_varlist, belong, nullptr, env);
    activity->is_guard_call = true;
    pushActivity(activity, env);
    return setFuncActivityToArg(gl->func, env);
}

void setArgCodeListToActivity(af_ArgCodeList *acl, af_Environment *env) {
    if (acl != nullptr) {
        setActivityBtStart(acl->code, env->activity);
        env->activity->run_in_func = acl->run_in_func;
    } else {
        setActivityBtStart(nullptr, env->activity);
        env->activity->run_in_func = false;
    }
}

bool setFuncActivityToArg(af_Object *func, af_Environment *env) {
    af_ObjectAPI *api = getObjectAPI(func);
    auto get_acl = (obj_funcGetArgCodeList *)findAPI("obj_funcGetArgCodeList", api);
    auto get_var_list = (obj_funcGetVarList *)findAPI("obj_funcGetVarList", api);
    af_VarList *func_varlist = nullptr;
    af_Object *belong = getBelongObject(func);

    if (get_var_list == nullptr) {
        pushMessageDown(makeERRORMessage(TYPE_ERROR, API_NOT_FOUND_INFO(obj_funcGetVarList), env), env);
        return false;
    }

    pthread_mutex_lock(env->activity->gc_lock);
    env->activity->func = func;
    env->activity->belong = belong;
    env->activity->status = act_func_arg;
    pthread_mutex_unlock(env->activity->gc_lock);

    /* 遇到错误时 get_acl 和 get_var_list 要自行设定msg */
    if (get_acl != nullptr) {
        if (!get_acl(getObjectID(func), func, &env->activity->acl_start, env->activity->bt_top, &env->activity->mark, env))  // 设置acl
            return false;
    } else
        env->activity->acl_start = nullptr;

    if (!get_var_list(getObjectID(func), func, &env->activity->func_varlist, env->activity->mark, env))  // 设置 func_var_list
        return false;

    env->activity->acl_done = env->activity->acl_start;
    setArgCodeListToActivity(env->activity->acl_start, env);
    return true;
}

bool setFuncActivityAddVar(af_Environment *env){
    af_ObjectAPI *api = getObjectAPI(env->activity->func);
    auto get_info = (obj_funcGetInfo *)findAPI("obj_funcGetInfo", api);
    auto get_arg_list = (obj_funcGetArgList *)findAPI("obj_funcGetArgList", api);
    af_FuncInfo *fi = nullptr;

    if (get_info == nullptr) {
        pushMessageDown(makeERRORMessage(TYPE_ERROR, API_NOT_FOUND_INFO(obj_funcGetInfo), env), env);
        return false;
    }

    /* env->activity->fi可能还存储着旧的FuncInfo(尾调用优化), 因此不能直接保存到 env->activity->fi 中 */
    if (!get_info(getObjectID(env->activity->func), env->activity->func, &fi, env->activity->bt_top, env->activity->mark, env))
        return false;
    if (fi == nullptr) {
        pushMessageDown(makeERRORMessage(API_RUN_ERROR, API_DONOT_GIVE(FuncInfo), env), env);
        return false;
    }

    if (fi->scope == super_pure_scope && env->activity->fi->embedded == super_embedded) {
        /* 超纯函数和超内嵌函数不得搭配使用 */
        pushMessageDown(makeERRORMessage(RUN_ERROR, PURE_EMBEDDED_INFO, env), env);
        return false;
    }

    pthread_mutex_lock(env->activity->gc_lock);
    if (fi->is_macro) {  // 是宏函数则保存变量空间
        env->activity->macro_varlist = env->activity->out_varlist;
        env->activity->count_macro_varlist = env->activity->count_out_varlist;
        env->activity->count_out_varlist = 0;
    }

    if (fi->scope != inline_scope) {
        env->activity->count_run_varlist = env->activity->count_out_varlist;
        env->activity->count_out_varlist = 0;
        env->activity->run_varlist = env->activity->out_varlist;
    } else if  (fi->scope == normal_scope) { // 使用函数变量空间
        env->activity->count_run_varlist = 0;
        env->activity->run_varlist = env->activity->func_varlist;
    } else if (fi->scope == pure_scope) {  // 纯函数只有 protect 变量空间
        env->activity->count_run_varlist = 1;
        env->activity->run_varlist = pushProtectVarList(nullptr, env);
    } else if (fi->scope == super_pure_scope) {  // 超纯函数没有变量空间, 因此不得为超内嵌函数(否则var_list就为nullptr了)
        env->activity->count_run_varlist = 0;
        env->activity->run_varlist = nullptr;
    }

    env->activity->func_varlist = nullptr;
    freeVarSpaceListCount(env->activity->count_out_varlist, env->activity->out_varlist);  // freeVarSpaceListCount 前释放, 避免死锁
    env->activity->count_out_varlist = 0;
    env->activity->out_varlist = nullptr;

    if (fi->embedded != super_embedded) {  // 不是超内嵌函数则引入一层新的变量空间
        /* 新层的变量空间应该属于belong而不是func */
        env->activity->run_varlist = pushNewVarList(env->activity->belong, env->activity->run_varlist, env);;
        env->activity->count_run_varlist++;
        gc_delVarListReference(env->activity->run_varlist, env);
    }
    pthread_mutex_unlock(env->activity->gc_lock);

    if (fi->var_this && env->activity->belong != nullptr) {
        if (!makeVarToVarSpaceList("this", 3, 3, 3, env->activity->belong,
                                   env->activity->run_varlist, env->activity->belong, env)) {
            pushMessageDown(makeERRORMessage(RUN_ERROR, IMPLICIT_SET_INFO(this), env), env);
            return false;
        }
    }

    if (fi->var_func && env->activity->func != nullptr) {
        if (!makeVarToVarSpaceList("func", 3, 3, 3, env->activity->func,
                                   env->activity->run_varlist, env->activity->belong, env)) {
            pushMessageDown(makeERRORMessage(RUN_ERROR, IMPLICIT_SET_INFO(func), env), env);
            return false;
        }
    }

    /* 计算参数 */
    if (get_arg_list != nullptr) {
        af_ArgList *al;
        if (!get_arg_list(getObjectID(env->activity->func), env->activity->func, &al, env->activity->acl_start,
                          env->activity->mark, env))
            return false;
        runArgList(al, env->activity->run_varlist, env);
        freeAllArgList(al, env);
    }

    if (fi->embedded == protect_embedded)
        setVarSpaceProtect(env->activity->belong, env->activity->run_varlist->vs, true);

    /* ArgCodeList 在此处被清理 */
    freeAllArgCodeList(env->activity->acl_start, env);
    env->activity->acl_start = nullptr;
    env->activity->acl_done = nullptr;

    /* 此处检查fi是否为nullptr, 不为(通常为尾调用优化)则释放fi */
    /* 旧的FuncBody延迟到此处才释放(freeFuncInfo释放FuncBody), 是因为获取函数参数的相关运算中可能会使用旧FuncBody中的代码 */
    /* 因为调用函数的代码是在旧FuncBody中的, 因此参数计算的相关代码也可能在旧FuncBody中 */
    /* 也就是说ArgCodeList, bt_top, bt_start中的代码可能是来自旧FuncBody的 */
    /* 所以他们要延迟到现在才被释放 */
    /* 而get_arg_list是最后一次使用旧FuncBody中的代码(bt_top), 因此此处可以释放 */
    if (env->activity->fi != nullptr)
        freeFuncInfo(env->activity->fi);  // 延迟到这里再释放, 主要是FuncBody中的bt可能会被使用
    env->activity->fi = fi;
    env->activity->body_next = fi->body;

    /* bt_top等的相关设定会在 setFuncActivityToNormal 中被进行 */
    if (setFuncActivityToNormal(env) == 0)
        return false;  // 运行结束, 且已写入msg
    return true;
}

static void initCallFuncInfo(af_CallFuncInfo *cfi, af_Environment *env) {
    cfi->mark = env->activity->mark;
    cfi->body_next = env->activity->body_next;

    cfi->belong = env->activity->belong;
    cfi->func = env->activity->func;
    cfi->var_list = env->activity->run_varlist;

    cfi->call_type = env->activity->call_type;
    cfi->is_gc_call = env->activity->is_gc_call;
    cfi->is_literal = env->activity->is_literal;
    cfi->is_obj_func = env->activity->is_obj_func;
    cfi->is_macro_call = env->activity->is_macro_call;
}

/*
 * 函数名: setFuncActivityToNormal
 * 目标: 获取下一步需要运行的结果
 * 返回  (0) 表示无下一步
 * 返回 (-1) 表示运行C函数, 并且设置了 process_msg_first
 * 返回  (1) 表示下一步运行Code
 * 返回  (2) 表示遇到未被替换的动态代码块
 */
int setFuncActivityToNormal(af_Environment *env){  // 获取函数的函数体
    int re;
    af_FuncBody *body = env->activity->body_next;
    env->activity->status = act_func_normal;
    setActivityBtTop(nullptr, env->activity);  // NORMAL期, bt_top将不被设定
    setActivityBtNext(nullptr, env->activity);

    if (body == nullptr)  // 已经没有下一步了 (原msg不释放)
        return 0;

    af_Activity *activity = env->activity;  // 防止在函数调用期间env->activity被修改
    env->activity->body_next = body->next;
    switch (body->type) {
        case func_body_c: {
            af_CallFuncInfo cfi;
            initCallFuncInfo(&cfi, env);

            af_FuncBody *new_fb = GET_SYMBOL(body->c_func)(&cfi, env);
            activity->process_msg_first++;  // 处理C函数通过msg_down返回的结果
            if (cfi.body_next != env->activity->body_next) {
                env->activity->body_next = cfi.body_next;
                freeAllFuncBody(new_fb);  // 不再允许设定 FuncBody
            } else {
                pushDynamicFuncBody(new_fb, body);
                activity->body_next = body->next;  // 添加新元素后要重新设定body_next的位置
            }
            re = -1;
            break;
        }
        case func_body_import:
            if (!pushImportActivity(body->code, nullptr, nullptr, env)) {
                pushMessageDown(makeERRORMessage(IMPORT_ERROR, IMPORT_OBJ_ERROR, env), env);
                activity->process_msg_first++;
                re = 2;
                break;
            }
            re = 1;
            break;
        case func_body_code:
            setActivityBtStart(body->code, env->activity);
            re = 1;
            break;
        default:
        case func_body_dynamic:
            pushMessageDown(makeERRORMessage(RUN_ERROR, FUNCBODY_ERROR_INFO, env), env);
            activity->process_msg_first++;
            re = 2;
            break;
    }

    /* 在最后一个aFunBody完毕后, mark被释放(因为此后不会再有函数需要使用Mark) */
    if (activity->body_next == nullptr)  // 最后一个aFunBody
        freeMark(activity);
    return re;
}


/*
 * 函数名: runTopMessageProcess
 * 目标: 运行顶层信息处理器
 */
void runTopMessageProcess(bool is_top, af_Environment *env) {
    af_Message **pmsg = &env->activity->msg_down;
    while (*pmsg != nullptr) {
        af_TopMsgProcess *mp = findTopMsgProcessFunc((*pmsg)->type, env);
        if (mp != nullptr) {
            GET_SYMBOL(mp->func)(*pmsg, is_top, env);
            *pmsg = freeMessage(*pmsg);
        } else
            pmsg = &((*pmsg)->next);
    }
}

static void freeMark(af_Activity *activity) {
    if (activity->type == act_func && activity->func != nullptr && activity->mark != nullptr) {
        auto func = (obj_funcFreeMask *)findAPI("obj_funcFreeMask", getObjectAPI(activity->func));
        if (func != nullptr)
            func(getObjectID(activity->func), activity->func, activity->mark);
        activity->mark = nullptr;
    }
}

/*
 * 函数名: popActivity
 * 目标: 释放Activity
 * 只有is_normal为false时才会检查释放mark
 */
void popActivity(bool is_normal, af_Message *msg, af_Environment *env) {
    if (env->activity->type == act_func || env->activity->type == act_top || env->activity->type == act_top_import) {
        if (msg != nullptr && env->activity->return_first) {  // msg有内容, 并且设定了返回首位, 则清除msg内容, 并压入首位(压入的代码在下面)
            if (EQ_STR(msg->type, "NORMAL")) {
                gc_delObjectReference(*(af_Object **) msg->msg, env);
                freeMessage(msg);
                msg = nullptr;
            }
        } else if (env->activity->return_first) {  // msg无内容, 并且设定了返回首位, 则检查msg_down是否有normal, 有则清除
            if (env->activity->msg_down != nullptr && EQ_STR(env->activity->msg_down->type, "NORMAL")) {
                af_Message *tmp = getFirstMessage(env);
                gc_delObjectReference(*(af_Object **) (tmp->msg), env);
                freeMessage(tmp);
            }
        }

        if (msg == nullptr && env->activity->return_first) {  // 如果首位
            if (env->activity->return_obj == nullptr)
                msg = makeERRORMessage(RUN_ERROR, RETURN_OBJ_NOT_FOUND_INFO, env);
            else {
                gc_addObjectReference(env->activity->return_obj, env);
                msg = makeNORMALMessage(env->activity->return_obj, env);
                env->activity->return_obj = nullptr;
            }
        }
    }

    if (msg != nullptr)
        pushMessageDown(msg, env);

    if (env->activity->type == act_top_import && /* import模式, 并且msg_down中有normal, 则把normal替换为belong */
        env->activity->msg_down != nullptr && EQ_STR(env->activity->msg_down->type, "NORMAL")) {
        af_Message *tmp = getFirstMessage(env);
        gc_addObjectReference(env->activity->belong, env);
        pushMessageDown(makeIMPORTMessage(env->activity->import_mark, env->activity->belong, env), env);  // 压入belong作为msg
        pushMessageDown(tmp, env);
    }

    /* 正常情况下在执行完最后一个FuncBody后释放mark, 非正常情况(即最后一个FuncBody可能还没执行)则需要在此释放mark */
    if (!is_normal)
        freeMark(env->activity);  // 遇到非正常退出时, 释放`mark`

    if (env->activity->type == act_top || env->activity->type == act_guardian) {// 顶层或gc/guardian层
        runTopMessageProcess((env->activity->type == act_top), env);
    } else {
        connectMessage(&(env->activity->msg_down), env->activity->prev->msg_down);
        env->activity->prev->msg_down = env->activity->msg_down;
        env->activity->msg_down = nullptr;

        /* popActivity必然设定process_msg_first */
        env->activity->prev->process_msg_first++;
    }

    if (env->activity->type != act_top)
        env->activity = freeActivity(env->activity, env);
    else
        freeActivityTop(env->activity);  // activity不被释放
}

static af_LiteralRegex *makeLiteralRegex(const char *pattern, const char *func, bool in_protect) {
    af_Regex *rg = makeRegex(pattern, nullptr);
    if (rg == nullptr)
        return nullptr;

    auto lr = calloc(1, af_LiteralRegex);
    lr->rg = rg;
    lr->pattern = strCopy(pattern);
    lr->func = strCopy(func);
    lr->in_protect = in_protect;
    return lr;
}

static af_LiteralRegex *freeLiteralRegex(af_LiteralRegex *lr) {
    af_LiteralRegex *next = lr->next;
    freeRegex(lr->rg);
    free(lr->func);
    free(lr->pattern);
    free(lr);
    return next;
}

static void freeAllLiteralRegex(af_LiteralRegex *lr) {
    while (lr != nullptr)
        lr = freeLiteralRegex(lr);
}

bool pushLiteralRegex(const char *pattern, const char *func, bool in_protect, af_Environment *env) {
    af_LiteralRegex *lr = makeLiteralRegex(pattern, func, in_protect);
    if (lr == nullptr)
        return false;
    lr->next = env->lr;
    env->lr = lr;
    return true;
}

/*
 * 函数名: checkLiteralCode
 * 目标: 检查对象是否为字面量
 * 注意: func被写入函数名, 但不是复制式写入
 */
bool checkLiteralCode(const char *literal, char **func, bool *in_protect, af_Environment *env) {
    for (af_LiteralRegex *lr = env->lr; lr != nullptr; lr = lr->next) {
        if (matchRegex(literal, lr->rg, nullptr) == 1) {
            if (func != nullptr)
                *func = lr->func;  // 不使用复制
            if (in_protect != nullptr)
                *in_protect = lr->in_protect;
            return true;
        }
    }
    return false;
}

static af_EnvironmentList *makeEnvironmentList(af_Environment *env) {
    static size_t id = 0;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    auto envl = calloc(1, af_EnvironmentList);
    envl->env = env;

    pthread_mutex_lock(&mutex);
    envl->id = id;
    id++;
    pthread_mutex_unlock(&mutex);
    return envl;
}

static bool freeEnvironmentList(af_EnvironmentList *envl, af_Environment *base) {
    pthread_mutex_lock(&base->thread_lock);

    if (envl->prev == nullptr) {
        base->env_list = envl->next;
    } else
        envl->prev->next = envl->next;
    if (envl->next != nullptr)
        envl->next->prev = envl->prev;
    free(envl);

    pthread_mutex_unlock(&base->thread_lock);
    return true;
}

bool freeEnvironmentListByEnv(af_Environment *env, af_Environment *base) {
    pthread_mutex_lock(&base->thread_lock);

    for (af_EnvironmentList *envl = base->env_list; envl != nullptr; envl = envl->next) {
        if (envl->env == env) {
            pthread_mutex_unlock(&base->thread_lock);
            return freeEnvironmentList(envl, base);
        }
    }
    pthread_mutex_unlock(&base->thread_lock);
    return false;
}

void pushEnvironmentList(af_Environment *env, af_Environment *base) {
    af_EnvironmentList *envl = makeEnvironmentList(env);
    pthread_mutex_lock(&base->thread_lock);
    envl->next = base->env_list;
    base->env_list = envl;
    pthread_mutex_unlock(&base->thread_lock);
}

af_ErrorInfo *makeErrorInfo(const char *type, const char *error, const char *note, FileLine line, FilePath path) {
    auto ei = calloc(1, af_ErrorInfo);
    ei->error_type = strCopy(type);
    ei->error = strCopy(error);
    pushErrorBacktracking(line, path, note, ei);
    return ei;
}

void freeErrorInfo(af_ErrorInfo *ei, af_Environment *env){
    free(ei->error_type);
    free(ei->error);
    if (ei->obj != nullptr)
        gc_delObjectReference(ei->obj, env);
    freeAllErrorBacktracking(ei->track);
    free(ei);
}

static void fprintfNote(FILE *file, const char *note) {
    char *ent = nullptr;
    while(true) {
        ent = const_cast<char *>(strchr(note, '\n'));
        if (ent != nullptr)
            *ent = NUL;
        fprintf(file, "   #note %s\n", note);
        if (ent == nullptr)  // 意味着是最后一部分`note`
            break;
        *ent = '\n';
        note = ent + 1;
    }
}

void fprintfErrorInfo(FILE *file, af_ErrorInfo *ei) {
    fprintf(file, "Error Traceback (most recent call last):\n");
    for (af_ErrorBacktracking *ebt = ei->track; ebt != nullptr; ebt = ebt->next) {
        fprintf(file, "  File \"%s\", line %d\n", ebt->file, ebt->line);
        if (ebt->note != nullptr)
            fprintfNote(file, ebt->note);
    }
    fprintf(file, "%s: \"%s\"\n", ei->error_type, ei->error);
    fflush(file);
}

static void fprintfNoteStderr(const char *note) {
    char *ent = nullptr;
    while(true) {
        ent = const_cast<char *>(strchr(note, '\n'));
        if (ent != nullptr)
            *ent = NUL;
        printf_stderr(0, "   #note %s\n", note);
        if (ent == nullptr)  // 意味着是最后一部分`note`
            break;
        *ent = '\n';
        note = ent + 1;
    }
}

void fprintfErrorInfoStderr(af_ErrorInfo *ei) {
    printf_stderr(0, "Error Traceback (most recent call last):\n");
    for (af_ErrorBacktracking *ebt = ei->track; ebt != nullptr; ebt = ebt->next) {
        printf_stderr(0, "  File \"%s\", line %d\n", ebt->file, ebt->line);
        if (ebt->note != nullptr)
            fprintfNoteStderr(ebt->note);
    }
    printf_stderr(0, "%s: \"%s\"\n", ei->error_type, ei->error);
    fflush(stderr);
}

static void fprintfNoteStdout(const char *note) {
    char *ent = nullptr;
    while(true) {
        ent = const_cast<char *>(strchr(note, '\n'));
        if (ent != nullptr)
            *ent = NUL;
        printf_stdout(0, "   #note %s\n", note);
        if (ent == nullptr)  // 意味着是最后一部分`note`
            break;
        *ent = '\n';
        note = ent + 1;
    }
}

void fprintfErrorInfoStdout(af_ErrorInfo *ei) {
    printf_stdout(0, "Error Traceback (most recent call last):\n");
    for (af_ErrorBacktracking *ebt = ei->track; ebt != nullptr; ebt = ebt->next) {
        printf_stdout(0, "  File \"%s\", line %d\n", ebt->file, ebt->line);
        if (ebt->note != nullptr)
            fprintfNoteStdout(ebt->note);
    }
    printf_stdout(0, "%s: \"%s\"\n", ei->error_type, ei->error);
    fflush(stdout);
}

static af_ErrorBacktracking *makeErrorBacktracking(FileLine line, FilePath file, const char *note) {
    auto ebt = calloc(1, af_ErrorBacktracking);
    ebt->line = line;
    if (file == nullptr)
        ebt->file = strCopy("unknown.aun.sys");
    else
        ebt->file = strCopy(file);
    if (note != nullptr)
        ebt->note = strCopy(note);
    return ebt;
}

static af_ErrorBacktracking *freeErrorBacktracking(af_ErrorBacktracking *ebt) {
    af_ErrorBacktracking *next = ebt->next;
    free(ebt->note);
    free(ebt->file);
    free(ebt);
    return next;
}

static void freeAllErrorBacktracking(af_ErrorBacktracking *ebt) {
    while(ebt != nullptr) {
        ebt = freeErrorBacktracking(ebt);
    }
}

void pushErrorBacktracking(FileLine line, FilePath file, const char *note, af_ErrorInfo *ei) {
    af_ErrorBacktracking *ebt = makeErrorBacktracking(line, file, note);
    ebt->next = ei->track;
    ei->track = ebt;
}

static char *getActivityInfoToBacktracking(af_Activity *activity){
    char info[512 + CODE_STR_MAX_SIZE] = "";

    /* strcat拼接的字符是可控的, 因此不需要使用安全函数 */
    if (activity->type == act_guardian) {
        strcat(info, "guardian-activity;");
        return strCopy(info);
    } else if (activity->type == act_top)
        strcat(info, "top-activity;");
    else if (activity->type == act_top_import)
        strcat(info, "top-import-activity;");
    else if (activity->is_execution)
        strcat(info, "execution-activity;");
    else if (activity->is_gc_call)
        strcat(info, "gc-destruct-function-call-activity;");
    else
        strcat(info, "function-call-activity;");

    switch (activity->status) {
        case act_func_get:
            strcat(info, "\nfunc-get;");
            break;
        case act_func_arg:
            strcat(info, "\nfunc-arg;");
            if (activity->run_in_func)
                strcat(info, " run-in-function-var-space;");
            break;
        case act_func_normal:
            strcat(info, "\nrun-code;");
            if (activity->return_first)
                strcat(info, " return-first-result;");
            break;
        default:
            break;
    }

    if (activity->is_macro_call)
        strcat(info, "\nmacro-call;");

    if (activity->is_literal)
        strcat(info, "\nliteral-call;");

    if (activity->is_obj_func)
        strcat(info, "\nobject-function-call;");

    if (activity->optimization)
        strcat(info, "\ntail-call-optimization;");

    if (activity->bt_done != nullptr) {
        char *code = codeToStr(activity->bt_done, 1);
        if (code != nullptr) {
            strcat(info, "\ncode: ");
            strcat(info, code);
            free(code);
        }
    }

    return strCopy(info);
}

static char *getActivityTrackBackInfoToBacktracking(af_ActivityTrackBack *atb) {
    char info[512] = "backtracking;";

    /* strcat拼接的字符是可控的, 因此不需要使用安全函数 */
    if (atb->is_execution)
        strcat(info, "\nexecution-activity;");
    else if (atb->is_gc_call)
        strcat(info, "\ngc-destruct-function-call-activity;");
    else
        strcat(info, "\nfunction-call-activity;");


    switch (atb->status) {
        case act_func_get:
            strcat(info, "\nfunc-get;");
            break;
        case act_func_arg:
            strcat(info, "\nfunc-arg;");
            if (atb->run_in_func)
                strcat(info, " run-in-function-var-space;");
            break;
        case act_func_normal:
            strcat(info, "\nrun-code;");
            if (atb->return_first)
                strcat(info, " return-first-result;");
            break;
        default:
            break;
    }

    if (atb->is_macro_call)
        strcat(info, "\nmacro-call;");

    if (atb->is_literal)
        strcat(info, "\nliteral-call;");

    if (atb->is_obj_func)
        strcat(info, "\nobject-function-call;");

    if (atb->optimization)
        strcat(info, "\ntail-call-optimization;");

    return strCopy(info);
}

/**
 * 生成用于类型为IMPORT的Message的信息的数据
 * 注意: obj必须添加 gc_addReference
 * @param mark
 * @param obj
 * @param env
 * @return
 */
af_ImportInfo *makeImportInfo(const char *mark, af_Object *obj, af_Environment *env){
    auto ii = calloc(1, af_ImportInfo);
    if (mark != nullptr)
        ii->mark = strCopy(mark);
    ii->obj = obj;
    return ii;
}

void freeImportInfo(af_ImportInfo *ii, af_Environment *env){
    free(ii->mark);
    if (ii->obj != nullptr)
        gc_delObjectReference(ii->obj, env);
    free(ii);
}

/**
 * 创建守护器函数调用列表
 * 注意: obj和func 必须添加 gc_addReference
 * @param obj func的belong (可为nullptr)
 * @param func 调用的函数
 * @param env 运行环境
 * @return
 */
static af_GuardianList *makeGuardianList(af_Object *obj, af_Object *func, af_Environment *env){
    auto gl = calloc(1, af_GuardianList);
    gl->obj = obj;
    gl->func = func;
    return gl;
}

static af_GuardianList *freeGuardianList(af_GuardianList *gl, af_Environment *env){
    af_GuardianList *next = gl->next;
    if (gl->obj != nullptr)
        gc_delObjectReference(gl->obj, env);
    gc_delObjectReference(gl->func, env);
    free(gl);
    return next;
}

static void freeAllGuardianList(af_GuardianList *gl, af_Environment *env){
    while (gl != nullptr)
        gl = freeGuardianList(gl, env);
}

af_GuardianList **pushGuardianList(af_Object *obj, af_Object *func, af_GuardianList **pgl, af_Environment *env){
    *pgl = makeGuardianList(obj, func, env);
    return &((*pgl)->next);
}

af_GuardianList **contectGuardianList(af_GuardianList *new_gl, af_GuardianList **base) {
    while ((*base) != nullptr)
        base = &((*base)->next);
    *base = new_gl;
    while ((*base) != nullptr)
        base = &((*base)->next);
    return base;
}

void setGcMax(int32_t max, af_Environment *env) {
    pthread_rwlock_wrlock(&env->esv->lock);
    env->gc_max->num = max;
    pthread_rwlock_unlock(&env->esv->lock);
}

void setGcRun(enum GcRunTime grt, af_Environment *env) {
    pthread_rwlock_wrlock(&env->esv->lock);
    env->gc_runtime->num = grt;
    pthread_rwlock_unlock(&env->esv->lock);
}

int32_t getGcCount(af_Environment *env) {
    pthread_rwlock_rdlock(&env->esv->lock);
    int32_t res = env->gc_count->num;
    pthread_rwlock_unlock(&env->esv->lock);
    return res;
}

void GcCountAdd1(af_Environment *env) {
    pthread_rwlock_wrlock(&env->esv->lock);
    env->gc_count->num++;
    pthread_rwlock_unlock(&env->esv->lock);
}

void GcCountToZero(af_Environment *env) {
    pthread_rwlock_wrlock(&env->esv->lock);
    env->gc_count->num = 0;
    pthread_rwlock_unlock(&env->esv->lock);
}

void setArgc(int argc, af_Environment *env) {
    pthread_rwlock_wrlock(&env->esv->lock);
    env->argc->num = argc;
    pthread_rwlock_unlock(&env->esv->lock);
}

int32_t getGcMax(af_Environment *env) {
    pthread_rwlock_rdlock(&env->esv->lock);
    int32_t res = env->gc_max->num;
    pthread_rwlock_unlock(&env->esv->lock);
    return res;
}

enum GcRunTime getGcRun(af_Environment *env) {
    pthread_rwlock_rdlock(&env->esv->lock);
    auto ret = (enum GcRunTime)env->gc_runtime->num;
    pthread_rwlock_unlock(&env->esv->lock);
    return ret;
}
int getArgc(af_Environment *env) {
    pthread_rwlock_rdlock(&env->esv->lock);
    int res = env->argc->num;
    pthread_rwlock_unlock(&env->esv->lock);
    return res;
}

af_Object *getCoreGlobal(af_Environment *env) {
    return env->global;
}

af_Object *getGlobal(af_Environment *env) {
    af_Activity *activity = env->activity;
    for(NULL; activity != nullptr; activity = activity->prev) {
        if (activity->type == act_top || activity->type == act_top_import) {
            gc_addObjectReference(activity->belong, env);
            return activity->belong;
        }
    }

    gc_addObjectReference(env->global, env);
    return env->global;
}

af_Object *getBelong(af_Environment *env) {
    if (env == nullptr || env->activity == nullptr)
        return nullptr;
    return env->activity->belong;
}

FilePath getActivityFile(af_Environment *env){
    if (env == nullptr || env->activity == nullptr)
        return nullptr;
    return env->activity->file;
}

FileLine getActivityLine(af_Environment *env){
    if (env == nullptr || env->activity == nullptr)
        return 0;
    return env->activity->line;
}

char *getErrorType(af_ErrorInfo *ei) {
    return ei->error_type;
}

char *getError(af_ErrorInfo *ei) {
    return ei->error;
}

char *getImportMark(af_ImportInfo *ii) {
    return ii->mark;
}

af_Object *getImportObject(af_ImportInfo *ii, af_Environment *env){
    af_Object *obj = ii->obj;
    if (obj == nullptr)
        return nullptr;
    ii->obj = nullptr;
    return obj;
}

af_VarList *getRunVarSpaceList(af_Environment *env) {
    return env->activity->run_varlist;
}

int isCoreExit(af_Environment *env) {
    enum af_CoreStatus status = getCoreStatus(env);
    if (status == core_exit)
        return 1;
    else if (status == core_stop)
        return -1;
    return 0;
}

bool getErrorStd(af_Environment *env) {
    pthread_rwlock_rdlock(&env->esv->lock);
    bool res = env->error_std->num != 0;  // true-stderr, false-stdout
    pthread_rwlock_unlock(&env->esv->lock);
    return res;
}

int32_t getCoreExitCode(af_Environment *env) {
    pthread_rwlock_rdlock(&env->esv->lock);
    int res = env->exit_code_->num;
    pthread_rwlock_unlock(&env->esv->lock);
    return res;
}

af_Object *getActivityFunc(af_Environment *env) {
    if (env->activity == nullptr || env->activity->type == act_guardian)
        return nullptr;
    return env->activity->func;
}

size_t getEnviromentSonCount(af_Environment *env) {
    pthread_mutex_lock(&env->thread_lock);
    size_t res = 0;
    for (af_EnvironmentList *envl = env->env_list; envl != nullptr; envl = envl->next)
        res++;
    pthread_mutex_unlock(&env->thread_lock);
    return res;
}

enum af_CoreStatus getCoreStatus(af_Environment *env) {
    pthread_mutex_lock(&env->status_lock);
    enum af_CoreStatus res = env->status;
    pthread_mutex_unlock(&env->status_lock);
    return res;
}

/**
 * 线程外部 指示线程结束
 * @param env
 */
void setEnviromentExit_out(af_Environment *env) {
    pthread_mutex_lock(&env->thread_lock);
    env->son_exit = true;
    if (env->monitor != nullptr)
        pthread_cond_signal(&env->monitor->cond);
    pthread_mutex_unlock(&env->thread_lock);
}

bool isEnviromentExit(af_Environment *env) {
    pthread_mutex_lock(&env->thread_lock);
    bool res = env->son_exit == true;  // 线程外部指示线程结束
    af_Environment *base = env->base;
    pthread_mutex_unlock(&env->thread_lock);
    if (res)
        return true;
    else if (env == base)
        return false;

    enum af_CoreStatus status = getCoreStatus(base);
    res = status == core_exit || status == core_normal_gc;  // 主线程结束
    return res;
}

/**
 * 等待次线程结束
 * @param env
 */
void waitForEnviromentExit(af_Environment *env) {
    pthread_mutex_lock(&env->status_lock);
    env->status = core_exit;  // 不需要设置 exit_code
    pthread_mutex_unlock(&env->status_lock);

    if (env->gc_env != nullptr)
        setEnviromentExit_out(env->gc_env);  // 设置 gc 线程退出

    pthread_mutex_lock(&env->thread_lock);
    while (env->env_list != nullptr) {
        pthread_mutex_unlock(&env->thread_lock);
        safeSleep(0.1);
        pthread_mutex_unlock(&env->thread_lock);
    }
    pthread_mutex_unlock(&env->thread_lock);
}