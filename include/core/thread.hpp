#ifndef AFUN_THREAD_HPP
#define AFUN_THREAD_HPP

AFUN_CORE_EXPORT af_Environment *
startRunThread(af_Environment *env, af_VarSpace *vs, af_Code *code, bool not_copy_code, bool derive_tmp,
               bool derive_guardian, bool derive_lr, bool enable);
AFUN_CORE_EXPORT void startRunThread_(af_Environment *env, af_Code *code, bool not_copy_code);

#endif //AFUN_THREAD_HPP
