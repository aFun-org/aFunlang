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
        explicit Message(const std::string &type);
        virtual ~Message() = default;
        [[nodiscard]] const std::string &getType() const {return type;}
    };

    class MessageStream {
    protected:
        Message *stream;
        [[nodiscard]] virtual Message *_getMessage(const std::string &type) const;

    public:
        MessageStream();
        virtual ~MessageStream();

        template<class T>
        [[nodiscard]] T *getMessage(const std::string &type) const {
            Message *msg = this->_getMessage(type);
            T *ret = dynamic_cast<T*>(msg);
            return ret;
        }

        virtual Message *popMessage(const std::string &type);
        void pushMessage(Message *msg);
    };

    class UpMessage : public MessageStream {
    protected:
        Message *old;
    public:
        explicit UpMessage(const UpMessage *old);
        ~UpMessage() override;
        Message *popMessage(const std::string &type) override;
    };

    class DownMessage : public MessageStream {};
}


#endif //AFUN_MSG_HPP
