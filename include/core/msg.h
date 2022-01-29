#ifndef AFUN_MSG_H
#define AFUN_MSG_H
#include <list>
#include <mutex>
#include <map>
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Message {
    public:
        explicit inline Message() = default;
        virtual ~Message() = default;
        Message &operator=(const Message &)=delete;

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

    class AFUN_CORE_EXPORT UpMessage : public MessageStream {
    public:
        explicit UpMessage(const UpMessage *old=nullptr);
        ~UpMessage() override = default;

        template <typename Callable, typename...T>
        void forEachAll(Callable func, T...arg);

    protected:
        const UpMessage *old;
        [[nodiscard]] Message *_getMessage(const std::string &type) const override;
    };

    class AFUN_CORE_EXPORT DownMessage : public MessageStream {
    public:
        void joinMsg(DownMessage &msg);
    };

    class AFUN_CORE_EXPORT InterMessage : public MessageStream {
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

    class InterOutMessage : public InterMessage {
    public:
        template<class T>
        [[nodiscard]] T *getMessage(const std::string &type) const = delete;  // 对外不设置 getMessage 以避免线程问题
    };

    class InterInMessage : public InterMessage {

    };
}

#include "msg.inline.h"
#include "msg.template.h"

#endif //AFUN_MSG_H
