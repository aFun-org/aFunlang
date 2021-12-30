#ifndef AFUN_MSG_HPP
#define AFUN_MSG_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"
#include "core.hpp"
#include "list"

namespace aFuncore {
    AFUN_CORE_EXPORT class Message {
        friend class MessageStream;
        friend class UpMessage;
        friend class DownMessage;

        Message *next;  // 下一条消息
    public:
        const std::string type;  // 消息类型标注
        explicit Message(const std::string &type_) : type {type_}, next {nullptr} {}
        virtual ~Message() = default;
        Message &operator=(const Message &)=delete;
    };

    class TopMessage : public Message {
    public:
        explicit TopMessage(const std::string &type_) : Message(type_) {}
        virtual void topProgress() = 0;
    };

    AFUN_CORE_EXPORT class NormalMessage : public TopMessage {
        Object *obj;
    public:
        explicit NormalMessage(Object *obj_) : TopMessage("NORMAL"), obj {obj_} {}
        ~NormalMessage() override;
        void topProgress() override;
        Object *getObject() {return obj;}
    };

    AFUN_CORE_EXPORT class ErrorMessage : public TopMessage {
        Inter *inter;

        std::string error_type;
        std::string error_info;
        struct TrackBack{
            const StringFilePath path;
            FileLine line;
        };
        std::list<TrackBack> trackback;
    public:
        explicit ErrorMessage(const std::string &error_type_, const std::string &error_info_, Activation *activation);
        void topProgress() override;
        std::string getErrorType() {return error_type;}
        std::string getErrorInfo() {return error_info;}
    };

    AFUN_CORE_EXPORT class MessageStream {
    protected:
        Message *stream;
        [[nodiscard]] virtual Message *_getMessage(const std::string &type) const;
    public:
        MessageStream();
        virtual ~MessageStream();
        MessageStream &operator=(const MessageStream &)=delete;

        template<class T>
        [[nodiscard]] T *getMessage(const std::string &type) const {
            Message *msg = this->_getMessage(type);
            T *ret = dynamic_cast<T*>(msg);
            return ret;
        }

        virtual Message *popMessage(const std::string &type);
        void pushMessage(Message *msg);

        template <typename T>
        void forEach(void (*func)(Message *, T), T arg) {
            for (Message *msg = stream; msg != nullptr; msg = msg->next) {
                func(msg, arg);
            }
        }
    };

    AFUN_CORE_EXPORT class UpMessage : public MessageStream {
    protected:
        Message *old;
    public:
        explicit UpMessage(const UpMessage *old=nullptr);
        ~UpMessage() override;

        Message *popMessage(const std::string &type) override;
    };

    AFUN_CORE_EXPORT class DownMessage : public MessageStream {
    public:
        void joinMsg(DownMessage *msg);
    };
}


#endif //AFUN_MSG_HPP
