#ifndef AFUN_THREAD_H
#define AFUN_THREAD_H

AFUN_CORE_EXPORT af_Environment *
startRunThread(af_Environment *env, af_VarSpace *vs, af_Code *code, bool free_code, bool derive_tmp,
               bool derive_guardian, bool derive_lr, bool enable);
AFUN_CORE_EXPORT void startRunThread_(af_Environment *env, af_Code *code, bool free_code);

#endif //AFUN_THREAD_H
