#include "__env.h"
#include "__run.h"
#include "core_init.h"
#include "thread.h"

struct EnvCode {
    af_Environment *env;
    af_Code *code;
    bool not_copy_code;
};

static void *runThread(void *ec);

/**
 * 启动一个次线程运行代码
 * 注意: vs需要 gc_addReference
 * @param env 父线程env
 * @param vs 压入的变量空间
 * @param code 执行的代码
 */
af_Environment *startRunThread(af_Environment *env, af_VarSpace *vs, af_Code *code, bool not_copy_code, bool derive_tmp,
                               bool derive_guardian, bool derive_lr, bool enable){
    af_Environment *base = env->base;
    af_Environment *new = deriveEnvironment(derive_tmp, derive_guardian, derive_lr, enable, base);

    if (vs == NULL) {
        af_Object *obj = getGlobal(env);
        vs = makeVarSpace(obj, 3, 3, 3, env);
        gc_delReference(obj, env);
    }

    new->activity->run_varlist = pushVarList(vs, new->activity->run_varlist);
    new->activity->count_run_varlist++;
    gc_delReference(vs, base);

    if (enable)  // 如果未Enable, 则暂时不启动线程
        startRunThread_(new, code, not_copy_code);
    return new;
}


void startRunThread_(af_Environment *env, af_Code *code, bool not_copy_code){
    struct EnvCode *ec = calloc(1, sizeof(struct EnvCode));
    ec->env = env;
    ec->code = code;
    ec->not_copy_code = not_copy_code;

    pthread_t id;
    pthread_create(&id, NULL, runThread, ec);
    pthread_detach(id);
}


static void *runThread(void *ec) {
    af_Environment *env = ((struct EnvCode *)ec)->env;
    af_Code *code = ((struct EnvCode *)ec)->code;
    bool not_copy_code = ((struct EnvCode *)ec)->not_copy_code;
    free(ec);

    if (!not_copy_code)  // “非-不要复制代码” 即 “要复制代码”
        code = copyCode(code);

    writeInfoLog(aFunCoreLogger, "Thread start");
    iterCode(code, 0, env);

    writeInfoLog(aFunCoreLogger, "Thread free-code");
    freeAllCode(code);

    writeInfoLog(aFunCoreLogger, "Thread free-env");
    freeEnvironment(env);

    writeInfoLog(aFunCoreLogger, "Thread end");
    return NULL;
}
