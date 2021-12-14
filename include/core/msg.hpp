#ifndef AFUN_MSG_HPP
#define AFUN_MSG_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    class MessageStream;

    class Message {
        std::string type;  // 消息类型标注
        Message *next;  // 下一条消息

    friend class MessageStream;
    friend class UpMessage;

    public:
        AFUN_CORE_EXPORT explicit Message(const std::string &type);
        AFUN_CORE_EXPORT virtual ~Message() = default;
        [[nodiscard]] const std::string &getType() const {return type;}
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
    };

    class UpMessage : public MessageStream {
    protected:
        Message *old;
    public:
        AFUN_CORE_EXPORT explicit UpMessage(const UpMessage *old);
        AFUN_CORE_EXPORT ~UpMessage() override;
        AFUN_CORE_EXPORT Message *popMessage(const std::string &type) override;
    };

    class DownMessage : public MessageStream {};
}


#endif //AFUN_MSG_HPP
