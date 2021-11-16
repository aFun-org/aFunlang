#ifndef AFUN_THREAD_H
#define AFUN_THREAD_H

AFUN_CORE_EXPORT af_Environment *startRunThread(af_Environment *env, af_VarSpace *vs, af_Code *code,
                                                bool derive_tmp, bool derive_guardian, bool derive_lr, bool enable);
AFUN_CORE_EXPORT void tartRunThread_(af_Environment *env, af_Code *code);

#endif //AFUN_THREAD_H
