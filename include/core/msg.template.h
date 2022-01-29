﻿#ifndef AFUN_MSG_TEMPLATE_H
#define AFUN_MSG_TEMPLATE_H
#include "msg.h"

namespace aFuncore {
    template<class T>
    T *MessageStream::getMessage(const std::string &type) const {
        Message *msg = this->_getMessage(type);
        T *ret = dynamic_cast<T*>(msg);
        return ret;
    }


    template <typename Callable, typename...T>
    inline void MessageStream::forEach(Callable func, T...arg) {
        for (auto &msg : stream)
            func(msg.second, arg...);
    }

    template<typename Callable, typename... T>
    void UpMessage::forEachAll(Callable func, T... arg) {
        for (const UpMessage *up = this; up != nullptr; up = up->old)
            up->MessageStream::forEach(func, arg...);
    }
}

#endif //AFUN_MSG_TEMPLATE_H
