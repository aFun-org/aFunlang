#ifndef AFUN_RT_MESSAGE_INLINE_H
#define AFUN_RT_MESSAGE_INLINE_H
#include "rt-message.h"

namespace aFunrt {
    NormalMessage::NormalMessage(NormalMessage &&msg) noexcept : obj {msg.obj}{
        msg.obj = nullptr;
    }

    aFuncore::Object *NormalMessage::getObject() {
        return obj;
    }

    ErrorMessage::ErrorMessage(ErrorMessage &&msg) noexcept
            : inter{msg.inter}, error_type{std::move(msg.error_type)},
              error_info{std::move(msg.error_info)}, trackback{std::move(msg.trackback)}{

    }

    std::string ErrorMessage::getErrorType() const {
        return error_type;
    }

    std::string ErrorMessage::getErrorInfo() const {
        return error_info;
    }

    const std::list<ErrorMessage::TrackBack> &ErrorMessage::getTrackBack() const {
        return trackback;
    }
}


#endif //AFUN_RT_MESSAGE_INLINE_H
