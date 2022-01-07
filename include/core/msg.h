#ifndef AFUN_MSG_H
#define AFUN_MSG_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core.h"
#include "list"

namespace aFuncore {
    class AFUN_CORE_EXPORT Message {
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

    class AFUN_CORE_EXPORT NormalMessage : public TopMessage {
        Object *obj;
    public:
        explicit NormalMessage(Object *obj_) : TopMessage("NORMAL"), obj {obj_} {}
        ~NormalMessage() override;
        void topProgress() override;
        Object *getObject() {return obj;}
    };

    class AFUN_CORE_EXPORT ErrorMessage : public TopMessage {
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

    class AFUN_CORE_EXPORT MessageStream {
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

    class AFUN_CORE_EXPORT UpMessage : public MessageStream {
    protected:
        Message *old;
    public:
        explicit UpMessage(const UpMessage *old=nullptr);
        ~UpMessage() override;

        Message *popMessage(const std::string &type) override;
    };

    class AFUN_CORE_EXPORT DownMessage : public MessageStream {
    public:
        void joinMsg(DownMessage *msg);
    };
}


#endif //AFUN_MSG_H
