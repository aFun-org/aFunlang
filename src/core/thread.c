#include "__env.h"
#include "__run.h"
#include "core_init.h"
#include "thread.h"

struct EnvCode {
    af_Environment *env;
    af_Code *code;
};

static void *runThread(void *ec);

/**
 * 启动一个次线程运行代码
 * @param env 父线程env
 * @param vs 压入的变量空间
 * @param code 执行的代码
 */
af_Environment *startRunThread(af_Environment *env, af_VarSpace *vs, af_Code *code,
                               bool derive_tmp, bool derive_guardian, bool derive_lr, bool enable) {
    af_Environment *base = env->base;
    af_Environment *new = deriveEnvironment(derive_tmp, derive_guardian, derive_lr, enable, base);

    if (vs == NULL)
        vs = makeVarSpace(getGlobal(env), 3, 3, 3, env);

    af_VarSpaceListNode *vsl = makeVarSpaceList(vs);
    vsl->next = new->activity->run_varlist;
    new->activity->run_varlist = vsl;
    new->activity->count_run_varlist++;
    if (!enable)  // 如果未Enable, 则暂时不启动线程
        return new;

    struct EnvCode *ec = calloc(1, sizeof(struct EnvCode));
    ec->env = new;
    ec->code = code;

    pthread_t id;
    pthread_create(&id, NULL, runThread, ec);
    pthread_detach(id);
    return new;
}


void tartRunThread_(af_Environment *env, af_Code *code) {
    struct EnvCode *ec = calloc(1, sizeof(struct EnvCode));
    ec->env = env;
    ec->code = code;

    pthread_t id;
    pthread_create(&id, NULL, runThread, ec);
    pthread_detach(id);
}


static void *runThread(void *ec) {
    af_Environment *env = ((struct EnvCode *)ec)->env;
    af_Code *code = ((struct EnvCode *)ec)->code;
    free(ec);

    writeInfoLog(aFunCoreLogger, "Thread start");
    iterCode(code, 0, env);

    writeInfoLog(aFunCoreLogger, "Thread free");
    freeEnvironment(env);

    writeInfoLog(aFunCoreLogger, "Thread end");
    return NULL;
}
