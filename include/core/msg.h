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

    public:
        const std::string type;  // 消息类型标注
        explicit inline Message(const std::string &type_);
        virtual ~Message() = default;
        Message &operator=(const Message &)=delete;

    private:
        Message *next;  // 下一条消息
    };

    class TopMessage : public Message {
    public:
        explicit inline TopMessage(const std::string &type_);
        virtual void topProgress() = 0;
    };

    class AFUN_CORE_EXPORT NormalMessage : public TopMessage {
    public:
        explicit inline NormalMessage(Object *obj_);
        ~NormalMessage() override;
        void topProgress() override;
        inline Object *getObject();

    private:
        Object *obj;
    };

    class AFUN_CORE_EXPORT ErrorMessage : public TopMessage {
    public:
        explicit ErrorMessage(const std::string &error_type_, const std::string &error_info_, Activation *activation);
        void topProgress() override;
        inline std::string getErrorType();
        inline std::string getErrorInfo();

    private:
        Inter &inter;

        std::string error_type;
        std::string error_info;
        struct TrackBack{
            const StringFilePath path;
            FileLine line;
        };
        std::list<TrackBack> trackback;
    };

    class AFUN_CORE_EXPORT MessageStream {
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

    protected:
        Message *stream;
        [[nodiscard]] virtual Message *_getMessage(const std::string &type) const;
    };

    class AFUN_CORE_EXPORT UpMessage : public MessageStream {
    public:
        explicit UpMessage(const UpMessage *old=nullptr);
        ~UpMessage() override;

        Message *popMessage(const std::string &type) override;

    protected:
        Message *old;
    };

    class AFUN_CORE_EXPORT DownMessage : public MessageStream {
    public:
        void joinMsg(DownMessage &msg);
    };

    class AFUN_CORE_EXPORT InterMessage : public MessageStream {
        std::mutex mutex;
    };
}

#include "msg.inline.h"
#include "msg.template.h"

#endif //AFUN_MSG_H
