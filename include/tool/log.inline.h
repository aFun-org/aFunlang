#ifndef AFUN_LOG_INLINE_H
#define AFUN_LOG_INLINE_H

namespace aFuntool {
    inline bool LogFactory::news(){
        return !init || log_buf != nullptr;
    }

    inline int LogFactory::wait(){
        return pthread_cond_wait(&cond, &mutex);
    }

    inline bool LogFactory::stop(){
        return !init && log_buf == nullptr;
    }
}

#endif //AFUN_LOG_INLINE_H
