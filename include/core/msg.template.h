#ifndef AFUN_MSG_TEMPLATE_H
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
        for (Message *msg = stream; msg != nullptr; msg = msg->next) {
            func(msg, arg...);
        }
    }
}

#endif //AFUN_MSG_TEMPLATE_H
