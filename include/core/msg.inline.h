#ifndef AFUN_MSG_INLINE_H
#define AFUN_MSG_INLINE_H
#include "msg.h"

namespace aFuncore {
    inline NormalMessage::NormalMessage(NormalMessage &&msg) noexcept : obj {msg.obj}{
        msg.obj = nullptr;
    }

    inline Object *NormalMessage::getObject() {
        return obj;
    }

    inline ErrorMessage::ErrorMessage(ErrorMessage &&msg) noexcept
        : error_type{std::move(msg.error_type)}, error_info{std::move(msg.error_info)},
          trackback{std::move(msg.trackback)}, inter{msg.inter}{

    }

    inline std::string ErrorMessage::getErrorType() const {
        return error_type;
    }

    inline std::string ErrorMessage::getErrorInfo() const {
        return error_info;
    }

    inline const std::list<ErrorMessage::TrackBack> &ErrorMessage::getTrackBack() const {
        return trackback;
    }
}

#endif //AFUN_MSG_INLINE_H
