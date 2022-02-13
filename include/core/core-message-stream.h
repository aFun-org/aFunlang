#ifndef AFUN_CORE_MESSAGE_STREAM_H
#define AFUN_CORE_MESSAGE_STREAM_H
#include <list>
#include <mutex>
#include <map>
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core-message.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT MessageStream {
    public:
        MessageStream() = default;
        virtual ~MessageStream();
        MessageStream(const MessageStream &)=delete;
        MessageStream &operator=(const MessageStream &)=delete;

        template<class T>
        [[nodiscard]] T *getMessage(const std::string &type) const;

        Message *popMessage(const std::string &type);
        void pushMessage(const std::string &type, Message *msg);

        template <typename Callable, typename...T>
        void forEach(Callable func, T...arg);

    protected:
        std::map<std::string, Message *> stream;
        [[nodiscard]] virtual Message *_getMessage(const std::string &type) const;
    };

    class AFUN_CORE_EXPORT UpMessageStream : public MessageStream {
    public:
        explicit UpMessageStream(const UpMessageStream *old=nullptr);
        ~UpMessageStream() override = default;

        template <typename Callable, typename...T>
        void forEachAll(Callable func, T...arg);

    protected:
        const UpMessageStream *old;
        [[nodiscard]] Message *_getMessage(const std::string &type) const override;
    };

    class AFUN_CORE_EXPORT DownMessageStream : public MessageStream {
    public:
        void joinMsg(DownMessageStream &msg);
    };

    class AFUN_CORE_EXPORT InterMessageStream : public MessageStream {
    public:
        Message *popFrontMessage(std::string &type);
        Message *popMessage(const std::string &type);
        void pushMessage(const std::string &type, Message *msg);

        template <typename Callable, typename...T>
        void forEach(Callable func, T...arg);

        template <typename Callable, typename...T>
        void forEachLock(Callable func, T...arg);
    private:
        std::mutex lock;
    };

    class InterOutMessageStream : public InterMessageStream {
    public:
        template<class T>
        [[nodiscard]] T *getMessage(const std::string &type) const = delete;  // 对外不设置 getMessage 以避免线程问题
    };

    class InterInMessageStream : public InterMessageStream {

    };
}

#include "core-message-stream.template.h"

#endif //AFUN_CORE_MESSAGE_STREAM_H
