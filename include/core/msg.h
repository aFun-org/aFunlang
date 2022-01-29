#ifndef AFUN_MSG_H
#define AFUN_MSG_H
#include <list>
#include <mutex>
#include "aFuntool.h"
#include "aFunCoreExport.h"

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

    class Object;
    class Activation;
    class Inter;

    class TopMessage : public virtual Message {
    public:
        virtual void topProgress(Inter &inter, Activation &activation) = 0;
    };

    class AFUN_CORE_EXPORT NormalMessage : public TopMessage {
    public:
        explicit inline NormalMessage(Object *obj_);
        inline NormalMessage(NormalMessage &&msg) noexcept;
        ~NormalMessage() override;
        void topProgress(Inter &inter, Activation &activation) override;
        inline Object *getObject();

    private:
        Object *obj;
    };

    class AFUN_CORE_EXPORT ErrorMessage : public TopMessage {
    public:
        struct TrackBack {
            const aFuntool::FilePath path;
            aFuntool::FileLine line;
        };

        explicit ErrorMessage(std::string error_type_, std::string error_info_, Activation *activation);
        inline ErrorMessage(ErrorMessage &&msg) noexcept;
        void topProgress(Inter &inter_, Activation &activation) override;
        [[nodiscard]] inline std::string getErrorType() const;
        [[nodiscard]] inline std::string getErrorInfo() const;
        [[nodiscard]] inline const std::list<TrackBack> &getTrackBack() const;

    private:
        Inter &inter;

        std::string error_type;
        std::string error_info;
        std::list<TrackBack> trackback;
    };

    class AFUN_CORE_EXPORT MessageStream {
    public:
        MessageStream();
        virtual ~MessageStream();
        MessageStream &operator=(const MessageStream &)=delete;

        template<class T>
        [[nodiscard]] T *getMessage(const std::string &type) const;

        Message *popMessage(const std::string &type);
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

        Message *popMessage(const std::string &type);

    protected:
        Message *old;
    };

    class AFUN_CORE_EXPORT DownMessage : public MessageStream {
    public:
        void joinMsg(DownMessage &msg);
    };

    class AFUN_CORE_EXPORT InterMessage : public MessageStream {
        std::mutex mutex;
    public:
        Message *popFrontMessage();
    };
}

#include "msg.inline.h"
#include "msg.template.h"

#endif //AFUN_MSG_H
