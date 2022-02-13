#ifndef AFUN_TOOL_LOG_INLINE_H
#define AFUN_TOOL_LOG_INLINE_H
#ifdef __cplusplus

#include "tool_log.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    Logger::Logger(LogFactory &factor, std::string id, LogLevel level, bool exit) noexcept
        : factor_{factor}, id_{std::move(id)}, level_{level}, exit_{exit} {

    }

#ifndef AFUN_TOOL_C
}
#endif

#endif
#endif //AFUN_TOOL_LOG_INLINE_H
