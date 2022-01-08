#ifndef AFUN_MSG_H
#define AFUN_MSG_H
#include <list>
#include <mutex>
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Message {
        friend class MessageStream;
        friend class UpMessage;
        friend class DownMessage;
        friend class InterMessage;

        Message *next;  // 下一条消息
    public:
        const std::string type;  // 消息类型标注
        explicit Message(const std::string &type_);
        virtual ~Message() = default;
        Message &operator=(const Message &)=delete;
    };

    class TopMessage : public Message {
    public:
        explicit TopMessage(const std::string &type_);
        virtual void topProgress() = 0;
    };

    class AFUN_CORE_EXPORT NormalMessage : public TopMessage {
        Object *obj;
    public:
        explicit NormalMessage(Object *obj_);
        ~NormalMessage() override;
        void topProgress() override;
        Object *getObject();
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
        std::string getErrorType();
        std::string getErrorInfo();
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
        [[nodiscard]] T *getMessage(const std::string &type) const;

        virtual Message *popMessage(const std::string &type);
        void pushMessage(Message *msg);

        template <typename Callable, typename...T>
        void forEach(Callable func, T...arg);
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

    class AFUN_CORE_EXPORT InterMessage : public MessageStream {
        std::mutex mutex;
    };
}

#include "msg.inline.h"
#include "msg.template.h"

#endif //AFUN_MSG_H
