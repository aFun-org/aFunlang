#ifndef AFUN_MSG_INLINE_H
#define AFUN_MSG_INLINE_H
#include "msg.h"

namespace aFuncore {
    inline Message::Message(const std::string &type_) : type {type_}, next {nullptr} {

    }

    inline NormalMessage::NormalMessage(Object *obj_) : Message("NORMAL"), obj {obj_} {

    }

    inline Object *NormalMessage::getObject() {
        return obj;
    }

    inline std::string ErrorMessage::getErrorType() {
        return error_type;
    }

    inline std::string ErrorMessage::getErrorInfo() {
        return error_info;
    }
}

#endif //AFUN_MSG_INLINE_H
