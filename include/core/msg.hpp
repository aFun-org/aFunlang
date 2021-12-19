#ifndef AFUN_MSG_HPP
#define AFUN_MSG_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    class MessageStream;
    class Message;
    class NormalMessage;
    class UpMessage;
    class DownMessage;
}

#include "value.hpp"

namespace aFuncore {
    class Message {
        friend class MessageStream;
        friend class UpMessage;
        friend class DownMessage;

        Message *next;  // 下一条消息
    public:
        const std::string type;  // 消息类型标注
        AFUN_CORE_EXPORT explicit Message(const std::string &type_) : type {type_}, next {nullptr} {}
        AFUN_CORE_EXPORT virtual ~Message() = default;
    };

    class TopMessage : public Message {
    public:
        explicit TopMessage(const std::string &type_) : Message(type_) {}
        virtual void topProgress()=0;
    };

    class NormalMessage : public TopMessage {
        Object *obj;
    public:
        AFUN_CORE_EXPORT explicit NormalMessage(Object *obj);
        AFUN_CORE_EXPORT ~NormalMessage() override;
        void topProgress() override;
    };

    class MessageStream {
    protected:
        Message *stream;
        [[nodiscard]] AFUN_CORE_EXPORT virtual Message *_getMessage(const std::string &type) const;

    public:
        AFUN_CORE_EXPORT MessageStream();
        AFUN_CORE_EXPORT virtual ~MessageStream();

        template<class T>
        [[nodiscard]] T *getMessage(const std::string &type) const {
            Message *msg = this->_getMessage(type);
            T *ret = dynamic_cast<T*>(msg);
            return ret;
        }

        virtual AFUN_CORE_EXPORT Message *popMessage(const std::string &type);
        AFUN_CORE_EXPORT void pushMessage(Message *msg);

        template <typename T>
        AFUN_CORE_EXPORT void forEach(void (*func)(Message *, T), T arg) {
            for (Message *msg = stream; msg != nullptr; msg = msg->next) {
                func(msg, arg);
            }
        }
    };

    class UpMessage : public MessageStream {
    protected:
        Message *old;
    public:
        AFUN_CORE_EXPORT explicit UpMessage(const UpMessage *old=nullptr);
        AFUN_CORE_EXPORT ~UpMessage() override;
        AFUN_CORE_EXPORT Message *popMessage(const std::string &type) override;
    };

    class DownMessage : public MessageStream {
    public:
        AFUN_CORE_EXPORT void joinMsg(DownMessage *msg);
    };
}


#endif //AFUN_MSG_HPP
