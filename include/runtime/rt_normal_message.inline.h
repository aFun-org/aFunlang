#ifndef AFUN_RT_NORMAL_MESSAGE_INLINE_H
#define AFUN_RT_NORMAL_MESSAGE_INLINE_H
#include "rt_normal_message.h"

namespace aFunrt {
    NormalMessage::NormalMessage(NormalMessage &&msg) noexcept : obj {msg.obj}{
        msg.obj = nullptr;
    }

    aFuncore::Object *NormalMessage::getObject() {
        return obj;
    }
}

#endif //AFUN_RT_NORMAL_MESSAGE_INLINE_H
