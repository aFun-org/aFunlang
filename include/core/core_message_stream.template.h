#ifndef AFUN_CORE_MESSAGE_STREAM_TEMPLATE_H
#define AFUN_CORE_MESSAGE_STREAM_TEMPLATE_H
#include "core_message_stream.h"

namespace aFuncore {
    template<class T>
    T *MessageStream::getMessage(const std::string &type) const {
        Message *msg = this->_getMessage(type);
        T *ret = dynamic_cast<T*>(msg);
        return ret;
    }


    template <typename Callable, typename...T>
    void MessageStream::forEach(Callable func, T...arg) {
        for (auto &msg : stream)
            func(msg.second, arg...);
    }

    template<typename Callable, typename... T>
    void UpMessageStream::forEachAll(Callable func, T... arg) {
        for (const UpMessageStream *up = this; up != nullptr; up = up->old)
            up->MessageStream::forEach(func, arg...);
    }

    template<typename Callable, typename... T>
    void InterMessageStream::forEach(Callable func, T... arg) {
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &msg : stream) {
            mutex.unlock();
            func(msg.second, arg...);
            mutex.lock();
        }
    }

    template<typename Callable, typename... T>
    void InterMessageStream::forEachLock(Callable func, T... arg) {
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &msg : stream)
            func(msg.second, arg...);
    }
}

#endif //AFUN_CORE_MESSAGE_STREAM_TEMPLATE_H
