#include "aFunCore.h"
#include "__env.h"
#include "__global_obj.h"
#include "__run.h"

/* Core 创建和释放 */
static af_Core *makeCore(enum GcRunTime grt);
static void freeCore(af_Environment *env);

/* Activity 创建和释放 */
static af_Activity *makeActivity(af_Message *msg_up, af_VarSpaceListNode *vsl, af_Object *belong);
static af_Activity *makeFuncActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                     af_VarSpaceListNode *vsl, af_Object *belong, af_Object *func);
static af_Activity *makeTopActivity(af_Code *bt_top, af_Code *bt_start, af_VarSpace *protect, af_Object *belong);
static af_Activity *makeTopImportActivity(af_Code *bt_top, af_Code *bt_start, af_VarSpace *protect, af_Object *belong, char *mark);
static af_Activity *makeGcActivity(gc_DestructList *dl, gc_DestructList **pdl, af_Environment *env);
static af_Activity *freeActivity(af_Activity *activity);
static void freeActivityTop(af_Activity *activity);
static void freeAllActivity(af_Activity *activity);
static void clearActivity(af_Activity *activity);

/* Activity 相关处理函数 */
static void freeMark(af_Activity *activity);
static void newActivity(af_Code *bt, const af_Code *next, bool return_first, af_Environment *env);

/* ActivityTrackBack 创建与释放 */
static af_ActivityTrackBack *makeActivityTrackBack(af_Activity *activity);
static af_ActivityTrackBack *freeActivityTrackBack(af_ActivityTrackBack *atb);
static void freeAllActivityTrackBack(af_ActivityTrackBack *atb);

/* 环境变量 创建与释放 */
static af_EnvVar *makeEnvVar(char *name, char *data);
static af_EnvVar *freeEnvVar(af_EnvVar *var);
static void freeAllEnvVar(af_EnvVar *var);
static void freeEnvVarSpace(af_EnvVarSpace *evs);

/* 顶层消息处理器 创建与释放 */
static af_TopMsgProcess *makeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func);
static af_TopMsgProcess *freeTopMsgProcess(af_TopMsgProcess *mp);
static void freeAllTopMsgProcess(af_TopMsgProcess *mp);

/* 顶层消息处理器 处理函数 */
static af_TopMsgProcess *findTopMsgProcessFunc(char *type, af_Environment *env);

/* LiteralData 创建与释放 */
static af_LiteralDataList *makeLiteralDataList(char *data);
static af_LiteralDataList *freeLiteralData_Pri(af_LiteralDataList *ld);

/* LiteralRegex 创建与释放 */
static af_LiteralRegex *makeLiteralRegex(char *pattern, char *func, bool in_protect);
static af_LiteralRegex *freeLiteralRegex(af_LiteralRegex *lr);
static void freeAllLiteralRegex(af_LiteralRegex *lr);

/* af_ErrorBacktracking 创建与释放 */
static af_ErrorBacktracking *makeErrorBacktracking(FileLine line, FilePath file, char *note);
static af_ErrorBacktracking *freeErrorBacktracking(af_ErrorBacktracking *ebt);
static void freeAllErrorBacktracking(af_ErrorBacktracking *ebt);

/* af_ErrorBacktracking 相关函数 */
static char *getActivityInfoToBacktracking(af_Activity *activity);
static char *getActivityTrackBackInfoToBacktracking(af_ActivityTrackBack *atb);
static void fprintfNote(FILE *file, char *note);

/* 内置顶层消息处理器 */
static void mp_NORMAL(af_Message *msg, bool is_gc, af_Environment *env);
static void mp_ERROR(af_Message *msg, bool is_gc, af_Environment *env);
static void mp_IMPORT(af_Message *msg, bool is_gc, af_Environment *env);

/* 变量检查函数 */
static bool isInfixFunc(af_Code *code, af_Environment *env);

static af_Core *makeCore(enum GcRunTime grt) {
    af_Core *core = calloc(1, sizeof(af_Core));
    core->status = core_creat;
    core->protect = makeVarSpaceByCore(NULL, 3, 3, 3, core);
    core->gc_run = grt;
    core->gc_count_max = DEFAULT_GC_COUNT_MAX;
    return core;
}

/*
 * 函数名: freeCore
 * 目标: 释放Core
 * 因为gc_freeAllValue需要env作为参数, 故使用env作为freeCore的参数
 */
static void freeCore(af_Environment *env) {
    printGCByCode(env->core);
    gc_freeAllValue(env);
    freeAllLiteralRegex(env->core->lr);
    free(env->core);
}

char setPrefix(size_t name, char prefix, af_Environment *env) {
    if (name >= PREFIX_SIZE)
        return '-';  // 表示未获取到prefix (NUL在Code中表示无prefix)
    char *prefix_ = findEnvVar(ev_sys_prefix, env);
    if (prefix_ == NULL || strlen(prefix_) < PREFIX_SIZE)
        return '-';
    switch (name) {
        case E_QUOTE:
            if (prefix == NUL && strchr(E_PREFIX, prefix) == NULL)
                prefix = '-';
            break;
        case B_EXEC:
        case B_EXEC_FIRST:
            if (prefix == NUL && strchr(B_PREFIX, prefix) == NULL)
                prefix = '-';
            break;
        default:
            break;
    }
    char old = prefix_[name];
    prefix_[name] = prefix;
    return old;
}

char getPrefix(size_t name, af_Environment *env) {
    if (name >= PREFIX_SIZE)
        return '-';  // 表示未获取到prefix (NUL在Code中表示无prefix)

    char *prefix = findEnvVar(ev_sys_prefix, env);
    if (prefix == NULL || strlen(prefix) < PREFIX_SIZE)
        return '-';
    return prefix[name];
}

af_VarSpace *getProtectVarSpace(af_Environment *env) {
    return env->core->protect;
}

/*
 * 函数名: getBaseObjectFromCore
 * 目标: 从VarSpace中获取一个量
 * 作用: 用于init初始化时在保护空间获得一些初始化对象
 */
af_Object *getBaseObjectFromCore(char *name, af_Core *core) {
    af_Var *var = findVarFromVarSpace(name, NULL, core->protect);
    if (var != NULL)
        return var->vn->obj;
    return NULL;
}

/*
 * 函数名: getBaseObject
 * 目标: getBaseObjectFromCore的对外接口
 */
af_Object *getBaseObject(char *name, af_Environment *env) {
    return getBaseObjectFromCore(name, env->core);
}

void setCoreStop(af_Environment *env) {
    if (env->core->status != core_exit)
        env->core->status = core_stop;
}

void setCoreExit(int exit_code, af_Environment *env) {
    env->core->status = core_exit;
    env->core->exit_code = exit_code;
}

void setCoreNormal(af_Environment *env) {
    if (env->core->status == core_exit || env->core->status == core_stop) {
        env->core->status = core_normal;
        env->core->exit_code = 0;
    }
}

static af_Activity *makeActivity(af_Message *msg_up, af_VarSpaceListNode *vsl, af_Object *belong) {
    af_Activity *activity = calloc(1, sizeof(af_Activity));
    activity->msg_up = msg_up;
    activity->msg_up_count = 0;
    activity->var_list = vsl;
    activity->new_vs_count = 0;
    activity->belong = belong;
    activity->line = 1;
    return activity;
}

static af_Activity *makeFuncActivity(af_Code *bt_top, af_Code *bt_start, bool return_first, af_Message *msg_up,
                                     af_VarSpaceListNode *vsl, af_Object *belong, af_Object *func) {
    af_Activity *activity = makeActivity(msg_up, vsl, belong);

    activity->type = act_func;
    activity->status = act_func_get;
    activity->func = func;

    setActivityBtTop(bt_top, activity);  // 非NORMAL期间, bt_top被设定
    setActivityBtStart(bt_start, activity);

    activity->return_first = return_first;
    return activity;
}

static af_Activity *makeTopActivity(af_Code *bt_top, af_Code *bt_start, af_VarSpace *protect, af_Object *belong) {
    af_Activity *activity = makeActivity(NULL, NULL, belong);

    activity->type = act_top;
    activity->status = act_func_normal;

    activity->new_vs_count = 2;
    activity->var_list = makeVarSpaceList(belong->data->var_space);
    activity->var_list->next = makeVarSpaceList(protect);

    setActivityBtTop(NULL, activity);  // top-activity直接就在normal, bt_top将不被设定
    setActivityBtStart(bt_start, activity);

    return activity;
}

static af_Activity *makeTopImportActivity(af_Code *bt_top, af_Code *bt_start, af_VarSpace *protect, af_Object *belong, char *mark) {
    af_Activity *activity = makeTopActivity(bt_top, bt_start, protect, belong);
    activity->type = act_top_import;
    if (mark != NULL)
        activity->import_mark = strCopy(mark);
    return activity;
}

static af_Activity *makeGcActivity(gc_DestructList *dl, gc_DestructList **pdl, af_Environment *env) {
    af_Activity *activity = makeActivity(NULL, NULL, env->core->global);
    activity->type = act_gc;

    activity->var_list = makeVarSpaceList(getProtectVarSpace(env));
    activity->new_vs_count = 1;

    activity->file = strCopy("gc.aun.sys");
    activity->line = 1;
    activity->dl = dl;
    activity->pdl = pdl;
    activity->dl_next = dl;
    return activity;
}

static af_Activity *freeActivity(af_Activity *activity) {
    af_Activity *prev = activity->prev;

    freeAllMessage(activity->msg_down);  // msg转移后需要将对应成员设置为NULL
    freeMessageCount(activity->msg_up_count, activity->msg_up);

    freeVarSpaceListCount(activity->new_vs_count, activity->var_list);
    free(activity->file);

    if (activity->type == act_gc) {
        if (activity->dl != NULL)
            freeAllDestructList(activity->dl);
    } else {
        // vsl 是引用自 var_list和func_var_list的 故不释放
        // func_var_list 是引用自函数的 故不释放
        freeVarSpaceListCount(activity->macro_vs_count, activity->macro_vsl);

        freeAllArgCodeList(activity->acl_start);
        if (activity->fi != NULL)
            freeFuncInfo(activity->fi);
        freeAllLiteralData(activity->ld);

        freeAllActivityTrackBack(activity->tb);
        free(activity->import_mark);
    }

    free(activity);
    return prev;
}

static void freeActivityTop(af_Activity *activity) {
    freeAllMessage(activity->msg_down);  // msg转移后需要将对应成员设置为NULL
    freeMessageCount(activity->msg_up_count, activity->msg_up);

    setActivityBtTop(NULL, activity);
    setActivityBtStart(NULL, activity);
    free(activity->file);
}

static void freeAllActivity(af_Activity *activity) {
    while (activity != NULL)
        activity = freeActivity(activity);
}

/*
 * 函数名: clearActivity
 * 目标: 尾调用优化时用于清理Activity.
 */
static void clearActivity(af_Activity *activity) {
    freeVarSpaceListCount(activity->macro_vs_count, activity->macro_vsl);
    /* acl在runArgList之后就被释放了 */
    /* acl在FuncBody暂时不释放 */

    activity->func_var_list = NULL;
    setActivityBtTop(NULL, activity);
    setActivityBtStart(NULL, activity);

    /* acl_start 在 setFuncActivityAddVar 时被释放 */
    activity->acl_start = NULL;
    activity->acl_done = NULL;

    /* activity->fi 暂时不清理, 直到setFuncActivityAddVar时才清理 */
    activity->body_next = NULL;

    /* mark在setFuncActivityToNormal被清理*/
    /* 只有FuncBody执行到最后一个(意味着Mark被清理)后才会有尾调用优化 */
    activity->mark = NULL;

    /* file和line都遗留 */
}

/*
 * 函数名: af_ActivityTrackBack
 * 目标: 把 activity 上的内容转移到新的 af_ActivityTrackBack 上
 */
static af_ActivityTrackBack *makeActivityTrackBack(af_Activity *activity) {
    af_ActivityTrackBack *atb = calloc(1, sizeof(af_ActivityTrackBack));
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
    while (atb != NULL)
        atb = freeActivityTrackBack(atb);
}

/*
 * 函数名: tailCallActivity
 * 目标: 记录ActivityTrackBack然后清除Activity
 */
static void tailCallActivity(af_Activity *activity) {
    af_ActivityTrackBack *atb = makeActivityTrackBack(activity);
    atb->next = activity->tb;
    activity->tb = atb;
    clearActivity(activity);
}

/*
 * 函数名: setActivityBtTop
 * 目标: 设置activity的bt_top, 并且设置行号
 * bt_start和bt_next会被设置为NULL
 */
void setActivityBtTop(af_Code *bt_top, af_Activity *activity) {
    activity->bt_top = bt_top;
    activity->bt_start = NULL;
    activity->bt_next = NULL;
    if (bt_top != NULL) {
        activity->line = bt_top->line;
        if (bt_top->path != NULL) {
            free(activity->file);
            activity->file = strCopy(bt_top->path);
        }
    } else
        activity->line = 1;
}

/*
 * 函数名: setActivityBtStart
 * 目标: 设置activity的bt_start, 并且设置行号
 * bt_next会被设置为NULL
 */
void setActivityBtStart(af_Code *bt_start, af_Activity *activity) {
    activity->bt_start = bt_start;
    activity->bt_next = bt_start;
    if (bt_start != NULL) {
        activity->line = bt_start->line;
        if (bt_start->path != NULL) {
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
    if (bt_next != NULL) {
        activity->line = bt_next->line;
        if (bt_next->path != NULL) {
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
    af_LiteralDataList *ld = calloc(1, sizeof(af_LiteralDataList));
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
    while (ld != NULL)
        ld = freeLiteralData_Pri(ld);
}

void pushLiteralData(char *data, af_Environment *env) {
    af_LiteralDataList *ld = makeLiteralDataList(data);
    ld->next = env->activity->ld;
    env->activity->ld = ld;
}

af_Message *makeMessage(char *type, size_t size) {
    af_Message *msg = calloc(1, sizeof(af_Message));
    msg->type = strCopy(type);
    if (size != 0)
        msg->msg = calloc(1, size);
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
    while (msg != NULL)
        msg = freeMessage(msg);
}

bool freeMessageCount(size_t count, af_Message *msg) {
    for (size_t i = count; i > 0; i--) {
        if (msg == NULL)  // 发生了错误
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

void *popMessageUpData(char *type, af_Environment *env) {
    for (af_Message **pmsg = &env->activity->msg_up; *pmsg != NULL; pmsg = &((*pmsg)->next)) {
        if (EQ_STR((*pmsg)->type, type))
            return (*pmsg)->msg;  // msg_up是只读的
    }
    return NULL;
}

af_Message *popMessageUp(af_Environment *env) {
    if (env->activity->msg_up_count == 0 || env->activity->msg_up == NULL)
        return NULL;

    af_Message *msg = env->activity->msg_up;
    env->activity->msg_up = msg->next;
    msg->next = NULL;
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

af_Message *popMessageDown(char *type, af_Environment *env) {
    for (af_Message **pmsg = &env->activity->msg_down; *pmsg != NULL; pmsg = &((*pmsg)->next)) {
        if (EQ_STR((*pmsg)->type, type)) {
            af_Message *msg = *pmsg;
            *pmsg = msg->next;
            msg->next = NULL;
            return msg;
        }
    }
    return NULL;
}

af_Message *getFirstMessage(af_Environment *env) {
    af_Message *msg = env->activity->msg_down;
    env->activity->msg_down = msg->next;
    msg->next = NULL;
    return msg;
}

void connectMessage(af_Message **base, af_Message *msg) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = msg;
}

af_Message *makeNORMALMessage(af_Object *obj) {
    af_Message *msg = makeMessage("NORMAL", sizeof(af_Object *));
    *(af_Object **)msg->msg = obj;
    gc_addReference(obj);
    return msg;
}

af_Message *makeERRORMessage(char *type, char *error, af_Environment *env) {
    char *info = getActivityInfoToBacktracking(env->activity);
    af_ErrorInfo *ei = makeErrorInfo(type, error, info, env->activity->line, env->activity->file);
    free(info);

    for (af_ActivityTrackBack *atb = env->activity->tb; atb != NULL; atb = atb->next) {
        info = getActivityTrackBackInfoToBacktracking(atb);
        pushErrorBacktracking(atb->line, atb->file, info, ei);
        free(info);
    }

    for (af_Activity *activity = env->activity->prev; activity != NULL; activity = activity->prev) {
        info = getActivityInfoToBacktracking(activity);
        pushErrorBacktracking(activity->line, activity->file, info, ei);
        free(info);

        for (af_ActivityTrackBack *atb = activity->tb; atb != NULL; atb = atb->next) {
            info = getActivityTrackBackInfoToBacktracking(atb);
            pushErrorBacktracking(atb->line, atb->file, info, ei);
            free(info);
        }
    }

    af_Message *msg = makeMessage("ERROR", sizeof(af_ErrorInfo *));
    *(af_ErrorInfo **)msg->msg = ei;
    return msg;
}

af_Message *makeERRORMessageFormat(char *type, af_Environment *env, const char *format, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, 1024, format, ap);
    va_end(ap);
    return makeERRORMessage(type, buf, env);;
}

af_Message *makeIMPORTMessage(char *mark, af_Object *obj) {
    af_Message *msg = makeMessage("IMPORT", sizeof(af_ImportInfo *));
    *(af_ImportInfo **)msg->msg = makeImportInfo(mark, obj);
    return msg;
}

static af_EnvVar *makeEnvVar(char *name, char *data) {
    af_EnvVar *var = calloc(1, sizeof(af_EnvVar));
    var->name = strCopy(name);
    var->data = strCopy(data);
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
    while (var != NULL)
        var = freeEnvVar(var);
}

static af_EnvVarSpace *makeEnvVarSpace(void) {
    af_EnvVarSpace *esv = calloc(1, sizeof(af_EnvVarSpace));
    return esv;
}

static void freeEnvVarSpace(af_EnvVarSpace *evs) {
    for (int i = 0; i < ENV_VAR_HASH_SIZE; i++)
        freeAllEnvVar(evs->var[i]);
    free(evs);
}

void setEnvVar(char *name, char *data, af_Environment *env) {
    time33_t index = time33(name) % ENV_VAR_HASH_SIZE;
    af_EnvVar **pvar = &env->esv->var[index];
    env->esv->count++;
    for (NULL; *pvar != NULL; pvar = &((*pvar)->next)) {
        if (EQ_STR((*pvar)->name, name)) {
            free((*pvar)->data);
            (*pvar)->data = strCopy(data);
            return;
        }
    }

    *pvar = makeEnvVar(name, data);
}

char *findEnvVar(char *name, af_Environment *env) {
    time33_t index = time33(name) % ENV_VAR_HASH_SIZE;
    af_EnvVar **pvar = &env->esv->var[index];

    for (NULL; *pvar != NULL; pvar = &((*pvar)->next)) {
        if (EQ_STR((*pvar)->name, name))
            return (*pvar)->data;
    }

    return NULL;
}

static void mp_NORMAL(af_Message *msg, bool is_gc, af_Environment *env) {
    if (msg->msg == NULL || *(af_Object **)msg->msg == NULL) {
        fprintf(stderr, "NORMAL msg: %p error\n", msg->msg);
        return;
    }
    gc_delReference(*(af_Object **)msg->msg);
    if (!is_gc)
        printf("NORMAL Point: %p\n", *(af_Object **)msg->msg);
}

static void mp_ERROR(af_Message *msg, bool is_gc, af_Environment *env) {
    if (msg->msg == NULL || *(af_ErrorInfo **)msg->msg == NULL) {
        printf("ERROR msg: %p error\n", msg->msg);
        return;
    }
    if (!is_gc)
        fprintfErrorInfo(stdout, *(af_ErrorInfo **)msg->msg);
    freeErrorInfo(*(af_ErrorInfo **)msg->msg);
}

static void mp_IMPORT(af_Message *msg, bool is_gc, af_Environment *env) {
    if (msg->msg == NULL || *(af_ImportInfo **)msg->msg == NULL) {
        printf("IMPORT msg: %p error\n", msg->msg);
        return;
    }
    af_ImportInfo *ii = *(af_ImportInfo **)msg->msg;
    if (ii->obj == NULL) {
        printf("IMPORT msg: %p do not get obj\n", msg->msg);
        return;
    }

    if (ii->mark != NULL) {
        makeVarToProtectVarSpace(ii->mark, 3, 3, 3, ii->obj, env);
        printf("IMPORT point: [%s] %p \n", ii->mark, ii->obj);
    } else
        printf("IMPORT point: <no-name> %p \n", ii->obj);
    freeImportInfo(ii);
}

af_Environment *makeEnvironment(enum GcRunTime grt) {
    af_Environment *env = calloc(1, sizeof(af_Environment));
    env->core = makeCore(grt);
    env->esv = makeEnvVarSpace();
    /* 生成global对象 */
    env->core->global = makeGlobalObject(env);
    addVarToProtectVarSpace(makeVar("global", 3, 3, 3, env->core->global, env), env);

    /* 设置默认prefix */
    char prefix[PREFIX_SIZE + 1] = "";
    prefix[E_QUOTE] = '\'';
    prefix[B_EXEC] = '\'';
    prefix[B_EXEC_FIRST] = '$';
    setEnvVar(ev_sys_prefix, prefix, env);

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

    env->core->status = core_init;
    env->activity = makeTopActivity(NULL, NULL, env->core->protect, env->core->global);
    return env;
}

void enableEnvironment(af_Environment *env) {
    env->core->protect->is_protect = true;
    env->core->status = core_normal;
}

void freeEnvironment(af_Environment *env) {
    bool res = true;
    if (env->core->status != core_creat)
        res = iterDestruct(10, env);

    freeAllActivity(env->activity);
    freeCore(env);
    freeEnvVarSpace(env->esv);
    freeAllTopMsgProcess(env->process);

    if (!res)
        printf("iterDestruct Error\n");
    free(env);
}

static af_TopMsgProcess *makeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func) {
    af_TopMsgProcess *mp = calloc(1, sizeof(af_TopMsgProcess));
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
    while (mp != NULL)
        mp = freeTopMsgProcess(mp);
}

static af_TopMsgProcess *findTopMsgProcessFunc(char *type, af_Environment *env) {
    af_TopMsgProcess *mp = env->process;
    for (NULL; mp != NULL; mp = mp->next) {
        if (EQ_STR(type, mp->type))
            return mp;
    }
    return NULL;
}

bool addTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func,
                      af_Environment *env) {
    af_TopMsgProcess *mp = findTopMsgProcessFunc(type, env);
    if (mp != NULL)
        return false;

    mp = makeTopMsgProcess(type, func);
    mp->next = env->process;
    env->process = mp;
    return true;
}

static void newActivity(af_Code *bt, const af_Code *next, bool return_first, af_Environment *env){
    if (next == NULL && env->activity->body_next == NULL && env->activity->type == act_func) {
        printf("Tail call optimization\n");
        tailCallActivity(env->activity);
        setActivityBtTop(bt, env->activity);
        env->activity->optimization = true;
        if (!env->activity->return_first)  // 若原本就有设置 return_first 则没有在设置的必要了, 因为该执行不会被返回
            env->activity->return_first = return_first;
    } else {
        af_Activity *activity = makeFuncActivity(bt, NULL, return_first, env->activity->msg_up,
                                                 env->activity->var_list, env->activity->belong,
                                                 env->activity->func);
        activity->prev = env->activity;
        env->activity = activity;
    }
}

/*
 * 函数名: isInfixFunc
 * 目标: 检查是否中缀调用函数
 */
static bool isInfixFunc(af_Code *code, af_Environment *env) {
    if (code == NULL || code->type != code_element || code->prefix == getPrefix(E_QUOTE, env))  // 检查是否element, 且无引用前缀
        return false;

    if (checkLiteralCode(code->element.data, NULL, NULL, env))  // 检查是否字面量
        return false;

    af_Var *var = findVarFromVarList(code->element.data, env->activity->belong, env->activity->var_list);
    if (var == NULL)
        return false;

    obj_isInfixFunc *func = findAPI("obj_isInfixFunc", var->vn->obj->data->api);
    if (func == NULL)
        return false;
    return func(var->vn->obj->data->id, var->vn->obj);
}

bool pushExecutionActivity(af_Code *bt, bool return_first, af_Environment *env) {
    af_Code *next;
    next = getCodeNext(bt);
    if (bt->type != code_block || bt->block.is_empty) {
        pushMessageDown(makeERRORMessage(SYNTAX_ERROR, NOT_CODE_INFO, env), env);
        return false;
    }

    setActivityBtNext(next, env->activity);

    newActivity(bt, next, return_first, env);
    setActivityBtStart(bt->next, env->activity);

    env->activity->status = act_func_normal;
    env->activity->is_execution = true;
    return true;
}

bool pushFuncActivity(af_Code *bt, af_Environment *env) {
    af_Code *next;
    af_Code *func;
    af_Object *parentheses_call = env->activity->parentheses_call;
    env->activity->parentheses_call = NULL;

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
            func = NULL;
            for (af_Code *code = bt->next; code != NULL; code = getCodeNext(code)) {
                if (isInfixFunc(code, env)) {
                    func = code;
                    break;
                }
            }
            if (func == NULL) {
                pushMessageDown(makeERRORMessage(CALL_ERROR, BRACKETS_FUNC_BODY_INFO, env), env);
                return false;
            }
            break;
        case parentheses:
            func = NULL;  // 小括号则不在需要匹配
            break;
        default:
            break;
    }

    setActivityBtNext(next, env->activity);  // 设置当前Activity的bt_next

    newActivity(bt, next, false, env);  // 添加新的Activity或尾调用优化
    setActivityBtStart(func, env->activity);

    env->activity->call_type = env->activity->bt_top->block.type;  // newActivity时会设置bt_top
    env->activity->status = act_func_get;
    if (env->activity->call_type == parentheses) { // 对于类前缀调用, 已经获得func的实际值了
        if (parentheses_call == NULL) {
            pushMessageDown(makeERRORMessage(CALL_ERROR, PARENTHESES_FUNC_BODY_INFO, env), env);
            return false;
        }
        return setFuncActivityToArg(parentheses_call, env);
    }
    return true;
}

bool pushLiteralActivity(af_Code *bt, char *data, af_Object *func, af_Environment *env) {
    setActivityBtNext(bt->next, env->activity);

    newActivity(bt, bt->next, false, env);
    env->activity->is_literal = true;
    pushLiteralData(strCopy(data), env);  // FuncBody的释放导致code和literal_data释放, 所以要复制
    return setFuncActivityToArg(func, env);
}

bool pushVariableActivity(af_Code *bt, af_Object *func, af_Environment *env) {
    setActivityBtNext(bt->next, env->activity);

    newActivity(bt, bt->next, false, env);
    env->activity->is_obj_func = true;
    return setFuncActivityToArg(func, env);
}

bool pushMacroFuncActivity(af_Object *func, af_Environment *env) {
    /* Macro是隐式调用, bt不移动 */
    /* 沿用activity */

    printf("Run macro\n");
    if (!freeVarSpaceListCount(env->activity->new_vs_count, env->activity->var_list)) { // 释放外部变量空间
        env->activity->new_vs_count = 0;
        pushMessageDown(makeERRORMessage(RUN_ERROR, FREE_VARSPACE_INFO, env), env);
        return false;
    }

    env->activity->var_list = env->activity->macro_vsl;
    env->activity->new_vs_count = env->activity->macro_vs_count;
    env->activity->macro_vs_count = 0;
    env->activity->is_macro_call = true;

    tailCallActivity(env->activity);  /* 隐式调用不设置 bt_top */
    return setFuncActivityToArg(func, env);
}

void pushGCActivity(gc_DestructList *dl, gc_DestructList **pdl, af_Environment *env) {
    for (af_Activity *tmp = env->activity; tmp != NULL; tmp = tmp->prev) {
        if (tmp->type == act_gc) {
            *(tmp->pdl) = dl;
            tmp->pdl = pdl;
            if (tmp->dl_next == NULL)  // 原dl_next已经运行到末端
                tmp->dl_next = dl;
            return;
        }
    }

    /* gc Activity 可能创建为顶层 activity, 故信息不能继承上一级(可能没有上一级) */
    af_Activity *activity = makeGcActivity(dl, pdl, env);

    activity->prev = env->activity;
    env->activity = activity;
}

bool pushImportActivity(af_Code *bt, af_Object **obj, char *mark, af_Environment *env) {
    af_Object *tmp = NULL;
    if (obj == NULL)
        obj = &tmp;

    if (*obj == NULL)
        *obj = makeGlobalObject(env);

    if (*obj == NULL)
        return false;

    af_Activity *activity = makeTopImportActivity(bt, bt, env->core->protect, *obj, mark);
    activity->prev = env->activity;
    env->activity = activity;
    return true;
}

bool pushDestructActivity(gc_DestructList *dl, af_Environment *env) {
    env->activity->dl_next = dl->next;

    /* 隐式调用不设置 bt_top */
    af_Activity *activity = makeFuncActivity(NULL, NULL, false, env->activity->msg_up,
                                             env->activity->var_list, env->activity->belong, NULL);
    activity->prev = env->activity;
    env->activity = activity;
    env->activity->is_gc_call = true;
    return setFuncActivityToArg(dl->func, env);
}

void setArgCodeListToActivity(af_ArgCodeList *acl, af_Environment *env) {
    if (acl != NULL) {
        setActivityBtStart(acl->code, env->activity);
        env->activity->run_in_func = acl->run_in_func;
    } else {
        setActivityBtStart(NULL, env->activity);
        env->activity->run_in_func = false;
    }
}

bool setFuncActivityToArg(af_Object *func, af_Environment *env) {
    obj_funcGetArgCodeList *get_acl = findAPI("obj_funcGetArgCodeList", func->data->api);
    obj_funcGetVarList *get_var_list = findAPI("obj_funcGetVarList", func->data->api);

    if (get_var_list == NULL) {
        pushMessageDown(makeERRORMessage(TYPE_ERROR, API_NOT_FOUND_INFO(obj_funcGetVarList), env), env);
        return false;
    }

    env->activity->func = func;
    env->activity->belong = getBelongObject(func);
    env->activity->status = act_func_arg;

    /* 遇到错误时 get_acl 和 get_var_list 要自行设定msg */
    if (get_acl != NULL) {
        if (!get_acl(func->data->id, func, &env->activity->acl_start, env->activity->bt_top, &env->activity->mark, env))  // 设置acl
            return false;
    } else
        env->activity->acl_start = NULL;

    if (!get_var_list(func->data->id, func, &env->activity->func_var_list, env->activity->mark, env))  // 设置 func_var_list
        return false;

    env->activity->acl_done = env->activity->acl_start;
    setArgCodeListToActivity(env->activity->acl_start, env);
    return true;
}

bool setFuncActivityAddVar(af_Environment *env){
    obj_funcGetInfo *get_info = findAPI("obj_funcGetInfo", env->activity->func->data->api);
    obj_funcGetArgList *get_arg_list = findAPI("obj_funcGetArgList", env->activity->func->data->api);
    af_FuncInfo *fi = NULL;

    if (get_info == NULL) {
        pushMessageDown(makeERRORMessage(TYPE_ERROR, API_NOT_FOUND_INFO(obj_funcGetInfo), env), env);
        return false;
    }

    /* env->activity->fi可能还存储着旧的FuncInfo(尾调用优化), 因此不能直接保存到 env->activity->fi 中 */
    if (!get_info(env->activity->func->data->id, env->activity->func, &fi, env->activity->bt_top, env->activity->mark, env))
        return false;
    if (fi == NULL) {
        pushMessageDown(makeERRORMessage(API_RUN_ERROR, API_DONOT_GIVE(FuncInfo), env), env);
        return false;
    }
    if (fi->scope == super_pure_scope && env->activity->fi->scope == super_embedded) {
        /* 超纯函数和超内嵌函数不得搭配使用 */
        pushMessageDown(makeERRORMessage(RUN_ERROR, PURE_EMBEDDED_INFO, env), env);
        return false;
    }

    if (fi->is_macro) {  // 是宏函数则保存变量空间
        env->activity->macro_vsl = env->activity->var_list;
        env->activity->macro_vs_count = env->activity->new_vs_count;
    } else if (fi->scope != inline_scope) {  // 非内联函数, 释放外部变量空间
        if (!freeVarSpaceListCount(env->activity->new_vs_count, env->activity->var_list)) {
            pushMessageDown(makeERRORMessage(RUN_ERROR, FREE_VARSPACE_INFO, env), env);  // 释放失败
            return false;
        }
    }

    if (fi->scope == normal_scope) {  // 使用函数变量空间
        env->activity->var_list = env->activity->func_var_list;
        env->activity->new_vs_count = 0;
    } else if (fi->scope == pure_scope) {  // 纯函数只有 protect 变量空间
        env->activity->var_list = makeVarSpaceList(env->core->protect);
        env->activity->new_vs_count = 0;
    } else if (fi->scope == super_pure_scope) {  // 超纯函数没有变量空间, 因此不得为超内嵌函数(否则var_list就为NULL了)
        env->activity->var_list = NULL;
        env->activity->new_vs_count = 0;
    }

    if (fi->embedded != super_embedded) {  // 不是超内嵌函数则引入一层新的变量空间
        /* 新层的变量空间应该属于belong而不是func */
        env->activity->var_list = pushNewVarList(env->activity->belong, env->activity->var_list, env);
        env->activity->new_vs_count++;
    }

    env->activity->func_var_list = NULL;

    if (fi->var_this && env->activity->belong != NULL) {
        if (!makeVarToVarSpaceList("this", 3, 3, 3, env->activity->belong,
                                   env->activity->var_list, env->activity->belong, env)) {
            pushMessageDown(makeERRORMessage(RUN_ERROR, IMPLICIT_SET_INFO(this), env), env);
            return false;
        }
    }

    if (fi->var_func && env->activity->func != NULL) {
        if (!makeVarToVarSpaceList("func", 3, 3, 3, env->activity->func,
                                   env->activity->var_list, env->activity->belong, env)) {
            pushMessageDown(makeERRORMessage(RUN_ERROR, IMPLICIT_SET_INFO(func), env), env);
            return false;
        }
    }

    /* 计算参数 */
    if (get_arg_list != NULL) {
        af_ArgList *al;
        if (!get_arg_list(env->activity->func->data->id, env->activity->func, &al, env->activity->acl_start, env->activity->mark, env))
            return false;
        runArgList(al, env->activity->var_list, env);
        freeAllArgList(al);
    }

    if (fi->embedded == protect_embedded)
        env->activity->var_list->vs->is_protect = true;

    /* ArgCodeList 在此处被清理 */
    freeAllArgCodeList(env->activity->acl_start);
    env->activity->acl_start = NULL;
    env->activity->acl_done = NULL;

    /* 此处检查fi是否为NULL, 不为(通常为尾调用优化)则释放fi */
    /* 旧的FuncBody延迟到此处才释放(freeFuncInfo释放FuncBody), 是因为获取函数参数的相关运算中可能会使用旧FuncBody中的代码 */
    /* 因为调用函数的代码是在旧FuncBody中的, 因此参数计算的相关代码也可能在旧FuncBody中 */
    /* 也就是说ArgCodeList, bt_top, bt_start中的代码可能是来自旧FuncBody的 */
    /* 所以他们要延迟到现在才被释放 */
    /* 而get_arg_list是最后一次使用旧FuncBody中的代码(bt_top), 因此此处可以释放 */
    if (env->activity->fi != NULL)
        freeFuncInfo(env->activity->fi);  // 延迟到这里再释放, 主要是FuncBody中的bt可能会被使用
    env->activity->fi = fi;
    env->activity->body_next = fi->body;

    /* bt_top等的相关设定会在 setFuncActivityToNormal 中被进行 */
    if (setFuncActivityToNormal(env) == 0)
        return false;  // 运行结束, 且已写入msg
    return true;
}

static void initCallFuncInfo(CallFuncInfo *cfi, af_Environment *env) {
    cfi->mark = env->activity->mark;
    cfi->belong = env->activity->belong;
    cfi->func = env->activity->func;
    cfi->var_list = env->activity->var_list;  // 传var_list而非vsl

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
    setActivityBtTop(NULL, env->activity);  // NORMAL期, bt_top将不被设定
    setActivityBtNext(NULL, env->activity);

    if (body == NULL)  // 已经没有下一步了 (原msg不释放)
        return 0;

    af_Activity *activity = env->activity;  // 防止在函数调用期间env->activity被修改
    env->activity->body_next = body->next;
    switch (body->type) {
        case func_body_c: {
            CallFuncInfo cfi;
            initCallFuncInfo(&cfi, env);

            af_FuncBody *new = GET_SYMBOL(body->c_func)(&cfi, env);
            activity->process_msg_first++;  // 处理C函数通过msg_down返回的结果
            pushDynamicFuncBody(new, body);
            activity->body_next = body->next;  // 添加新元素后要重新设定body_next的位置
            re = -1;
            break;
        }
        case func_body_import:
            if (!pushImportActivity(body->code, NULL, NULL, env)) {
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
    if (activity->body_next == NULL)  // 最后一个aFunBody
        freeMark(activity);
    return re;
}

/*
 * 函数名: runTopMessageProcess
 * 目标: 运行顶层信息处理器
 */
void runTopMessageProcess(bool is_gc, af_Environment *env) {
    af_Message **pmsg = &env->activity->msg_down;
    while (*pmsg != NULL) {
        af_TopMsgProcess *mp = findTopMsgProcessFunc((*pmsg)->type, env);
        if (mp != NULL) {
            GET_SYMBOL(mp->func)(*pmsg, is_gc, env);
            *pmsg = freeMessage(*pmsg);
        } else
            pmsg = &((*pmsg)->next);
    }
}

static void freeMark(af_Activity *activity) {
    if (activity->type == act_func && activity->func != NULL && activity->mark != NULL) {
        obj_funcFreeMask *func = findAPI("obj_funcFreeMask", activity->func->data->api);
        if (func != NULL)
            func(activity->func->data->id, activity->func, activity->mark);
        activity->mark = NULL;
    }
}

/*
 * 函数名: popActivity
 * 目标: 释放Activity
 * 只有is_normal为false时才会检查释放mark
 */
void popActivity(bool is_normal, af_Message *msg, af_Environment *env) {
    if (env->activity->type == act_func || env->activity->type == act_top || env->activity->type == act_top_import) {
        if (msg != NULL && env->activity->return_first) {  // msg有内容, 并且设定了返回首位, 则清除msg内容, 并压入首位(压入的代码在下面)
            if (EQ_STR(msg->type, "NORMAL")) {
                gc_delReference(*(af_Object **) msg->msg);
                freeMessage(msg);
                msg = NULL;
            }
        } else if (env->activity->return_first) {  // msg无内容, 并且设定了返回首位, 则检查msg_down是否有normal, 有则清除
            if (env->activity->msg_down != NULL && EQ_STR(env->activity->msg_down->type, "NORMAL")) {
                af_Message *tmp = getFirstMessage(env);
                gc_delReference(*(af_Object **) (tmp->msg));
                freeMessage(tmp);
            }
        }

        if (msg == NULL && env->activity->return_first) {  // 如果首位
            if (env->activity->return_obj == NULL)
                msg = makeERRORMessage(RUN_ERROR, RETURN_OBJ_NOT_FOUND_INFO, env);
            else
                msg = makeNORMALMessage(env->activity->return_obj);
        }
    }

    if (msg != NULL)
        pushMessageDown(msg, env);

    if (env->activity->type == act_top_import && /* import模式, 并且msg_down中有normal, 则把normal替换为belong */
        env->activity->msg_down != NULL && EQ_STR(env->activity->msg_down->type, "NORMAL")) {
        af_Message *tmp = getFirstMessage(env);
        pushMessageDown(makeIMPORTMessage(env->activity->import_mark, env->activity->belong), env);  // 压入belong作为msg
        pushMessageDown(tmp, env);
    }

    /* 正常情况下在执行完最后一个FuncBody后释放mark, 非正常情况(即最后一个FuncBody可能还没执行)则需要在此释放mark */
    if (!is_normal)
        freeMark(env->activity);  // 遇到非正常退出时, 释放`mark`

    if (env->activity->type == act_top || env->activity->type == act_gc) // 顶层或gc层
        runTopMessageProcess((env->activity->type == act_gc), env);
    else {
        connectMessage(&(env->activity->msg_down), env->activity->prev->msg_down);
        env->activity->prev->msg_down = env->activity->msg_down;
        env->activity->msg_down = NULL;

        /* popActivity必然设定process_msg_first */
        env->activity->prev->process_msg_first++;
    }

    if (env->activity->type != act_top)
        env->activity = freeActivity(env->activity);
    else
        freeActivityTop(env->activity);  // activity不被释放
}

static af_LiteralRegex *makeLiteralRegex(char *pattern, char *func, bool in_protect) {
    af_Regex *rg = makeRegex(pattern);
    if (rg == NULL)
        return NULL;

    af_LiteralRegex *lr = calloc(1, sizeof(af_LiteralRegex));
    lr->rg = rg;
    lr->func = strCopy(func);
    lr->in_protect = in_protect;
    return lr;
}

static af_LiteralRegex *freeLiteralRegex(af_LiteralRegex *lr) {
    af_LiteralRegex *next = lr->next;
    freeRegex(lr->rg);
    free(lr->func);
    free(lr);
    return next;
}

static void freeAllLiteralRegex(af_LiteralRegex *lr) {
    while (lr != NULL)
        lr = freeLiteralRegex(lr);
}

bool pushLiteralRegex(char *pattern, char *func, bool in_protect, af_Environment *env) {
    af_LiteralRegex *lr = makeLiteralRegex(pattern, func, in_protect);
    if (lr == NULL)
        return false;
    lr->next = env->core->lr;
    env->core->lr = lr;
    return true;
}

/*
 * 函数名: checkLiteralCode
 * 目标: 检查对象是否为字面量
 * 注意: func被写入函数名, 但不是复制式写入
 */
bool checkLiteralCode(char *literal, char **func, bool *in_protect, af_Environment *env) {
    for (af_LiteralRegex *lr = env->core->lr; lr != NULL; lr = lr->next) {
        if (matchRegex(literal, lr->rg) == 1) {
            if (func != NULL)
                *func = lr->func;  // 不使用复制
            if (in_protect != NULL)
                *in_protect = lr->in_protect;
            return true;
        }
    }
    return false;
}

af_ErrorInfo *makeErrorInfo(char *type, char *error, char *note, FileLine line, FilePath path) {
    af_ErrorInfo *ei = calloc(1, sizeof(af_ErrorInfo));
    ei->error_type = strCopy(type);
    ei->error = strCopy(error);
    pushErrorBacktracking(line, path, note, ei);
    return ei;
}

void freeErrorInfo(af_ErrorInfo *ei) {
    free(ei->error_type);
    free(ei->error);
    if (ei->obj != NULL)
        gc_delReference(ei->obj);
    freeAllErrorBacktracking(ei->track);
    free(ei);
}

static void fprintfNote(FILE *file, char *note) {
    char *ent = NULL;
    while(true) {
        ent = strchr(note, '\n');
        if (ent != NULL)
            *ent = NUL;
        fprintf(file, "   #note %s\n", note);
        if (ent == NULL)  // 意味着是最后一部分`note`
            break;
        *ent = '\n';
        note = ent + 1;
    }
}

void fprintfErrorInfo(FILE *file, af_ErrorInfo *ei) {
    fprintf(file, "Error Traceback (most recent call last):\n");
    for (af_ErrorBacktracking *ebt = ei->track; ebt != NULL; ebt = ebt->next) {
        fprintf(file, "  File \"%s\", line %d\n", ebt->file, ebt->line);
        if (ebt->note != NULL)
            fprintfNote(file, ebt->note);
    }
    fprintf(file, "%s: \"%s\"\n", ei->error_type, ei->error);
    fflush(file);
}

static af_ErrorBacktracking *makeErrorBacktracking(FileLine line, FilePath file, char *note) {
    af_ErrorBacktracking *ebt = calloc(1, sizeof(af_ErrorBacktracking));
    ebt->line = line;
    if (file == NULL)
        ebt->file = strCopy("unknown.aun.sys");
    else
        ebt->file = strCopy(file);
    if (note != NULL)
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
    while(ebt != NULL) {
        ebt = freeErrorBacktracking(ebt);
    }
}

void pushErrorBacktracking(FileLine line, FilePath file, char *note, af_ErrorInfo *ei) {
    af_ErrorBacktracking *ebt = makeErrorBacktracking(line, file, note);
    ebt->next = ei->track;
    ei->track = ebt;
}

static char *getActivityInfoToBacktracking(af_Activity *activity){
    char *info = NULL;
    if (activity->type == act_gc) {
        info = strJoin(info, "gc-activity;", true, false);
        return info;
    } else if (activity->type == act_top)
        info = strJoin(info, "top-activity;", true, false);
    else if (activity->type == act_top_import)
        info = strJoin(info, "top-import-activity;", true, false);
    else if (activity->is_execution)
        info = strJoin(info, "execution-activity;", true, false);
    else if (activity->is_gc_call)
        info = strJoin(info, "gc-destruct-function-call-activity;", true, false);
    else
        info = strJoin(info, "function-call-activity;", true, false);

    switch (activity->status) {
        case act_func_get:
            info = strJoin(info, "\nfunc-get;", true, false);
            break;
        case act_func_arg:
            info = strJoin(info, "\nfunc-arg;", true, false);
            if (activity->run_in_func)
                info = strJoin(info, " run-in-function-var-space;", true, false);
            break;
        case act_func_normal:
            info = strJoin(info, "\nrun-code;", true, false);
            if (activity->return_first)
                info = strJoin(info, " return-first-result;", true, false);
            break;
        default:
            break;
    }

    if (activity->is_macro_call)
        info = strJoin(info, "\nmacro-call;", true, false);

    if (activity->is_literal)
        info = strJoin(info, "\nliteral-call;", true, false);

    if (activity->is_obj_func)
        info = strJoin(info, "\nobject-function-call;", true, false);

    if (activity->optimization)
        info = strJoin(info, "\ntail-call-optimization;", true, false);

    return info;
}

static char *getActivityTrackBackInfoToBacktracking(af_ActivityTrackBack *atb) {
    char *info = "backtracking;";
    if (atb->is_execution)
        info = strJoin(info, "\nexecution-activity;", false, false);
    else if (atb->is_gc_call)
        info = strJoin(info, "\ngc-destruct-function-call-activity;", false, false);
    else
        info = strJoin(info, "\nfunction-call-activity;", false, false);

    switch (atb->status) {
        case act_func_get:
            info = strJoin(info, "\nfunc-get;", true, false);
            break;
        case act_func_arg:
            info = strJoin(info, "\nfunc-arg;", true, false);
            if (atb->run_in_func)
                info = strJoin(info, " run-in-function-var-space;", true, false);
            break;
        case act_func_normal:
            info = strJoin(info, "\nrun-code;", true, false);
            if (atb->return_first)
                info = strJoin(info, " return-first-result;", true, false);
            break;
        default:
            break;
    }

    if (atb->is_macro_call)
        info = strJoin(info, "\nmacro-call;", true, false);

    if (atb->is_literal)
        info = strJoin(info, "\nliteral-call;", true, false);

    if (atb->is_obj_func)
        info = strJoin(info, "\nobject-function-call;", true, false);

    if (atb->optimization)
        info = strJoin(info, "\ntail-call-optimization;", true, false);

    return info;
}

af_ImportInfo *makeImportInfo(char *mark, af_Object *obj) {
    af_ImportInfo *ii = calloc(1, sizeof(af_ImportInfo));
    if (mark != NULL)
        ii->mark = strCopy(mark);
    ii->obj = obj;
    gc_addReference(obj);
    return ii;
}

void freeImportInfo(af_ImportInfo *ii) {
    free(ii->mark);
    if (ii->obj != NULL)
        gc_delReference(ii->obj);
    free(ii);
}

void setGcMax(size_t max, af_Environment *env) {
    env->core->gc_count_max = max;
}

void setGcRun(enum GcRunTime grt, af_Environment *env) {
    env->core->gc_run = grt;
}

size_t getGcCount(af_Environment *env) {
    return env->core->gc_count;
}

size_t getGcMax(af_Environment *env) {
    return env->core->gc_count_max;
}

enum GcRunTime getGcRun(af_Environment *env) {
    return env->core->gc_run;
}

af_Object *getCoreGlobal(af_Environment *env) {
    return env->core->global;
}

af_Object *getGlobal(af_Environment *env) {
    af_Activity *activity = env->activity;
    for (NULL; activity != NULL; activity = activity->prev) {
        if (activity->type == act_top || activity->type == act_top_import)
            return activity->belong;
    }
    return env->core->global;
}

af_Object *getBelong(af_Environment *env) {
    if (env == NULL || env->activity == NULL)
        return NULL;
    return env->activity->belong;
}

FilePath getActivityFile(af_Environment *env){
    if (env == NULL || env->activity == NULL)
        return NULL;
    return env->activity->file;
}

FileLine getActivityLine(af_Environment *env){
    if (env == NULL || env->activity == NULL)
        return 0;
    return env->activity->line;
}

af_Object *getMsgNormalData(af_Message *msg) {
    if (!EQ_STR("NORMAL", msg->type))
        return NULL;
    af_Object *obj = *(af_Object **)msg->msg;
    gc_delReference(obj);
    *(af_Object **)msg->msg = NULL;
    return obj;
}

af_ErrorInfo *getMsgErrorInfo(af_Message *msg) {
    if (!EQ_STR("ERROR", msg->type))
        return NULL;
    af_ErrorInfo *ei = *(af_ErrorInfo **)msg->msg;
    return ei;
}

af_ImportInfo *getMsgImportInfo(af_Message *msg) {
    if (!EQ_STR("IMPORT", msg->type))
        return NULL;
    af_ImportInfo *ii = *(af_ImportInfo **)msg->msg;
    return ii;
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

af_Object *getImportObject(af_ImportInfo *ii) {
    af_Object *obj = ii->obj;
    if (obj == NULL)
        return NULL;
    ii->obj = NULL;
    gc_delReference(obj);
    return obj;
}

af_VarSpaceListNode *getRunVarSpaceList(af_Environment *env) {
    if (env->activity->type == act_gc)
        return env->activity->var_list;
    else
        return env->activity->vsl;
}

int isCoreExit(af_Environment *env) {
    if (env->core->status == core_exit)
        return 1;
    else if (env->core->status == core_stop)
        return -1;
    return 0;
}
