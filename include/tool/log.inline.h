#ifndef AFUN_LOG_INLINE_H
#define AFUN_LOG_INLINE_H
#include "log.h"

namespace aFuntool {
    inline aFuntool::Logger::Logger(LogFactory &factor, std::string id, LogLevel level, bool exit) noexcept
        : factor_{factor}, id_{std::move(id)}, level_{level}, exit_{exit} {

    }
}

#endif //AFUN_LOG_INLINE_H
