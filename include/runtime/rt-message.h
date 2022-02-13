#ifndef AFUN_RT_MESSAGE_H
#define AFUN_RT_MESSAGE_H
#include "aFunRuntimeExport.h"
#include "aFuncore.h"

namespace aFunrt {
    class TopMessage : public virtual aFuncore::Message {
    public:
        virtual void topProgress(aFuncore::Inter &inter, aFuncore::Activation &activation) = 0;
    };

    class AFUN_RT_EXPORT NormalMessage : public TopMessage {
    public:
        explicit NormalMessage(aFuncore::Object *obj_);
        AFUN_INLINE NormalMessage(NormalMessage &&msg) noexcept;
        ~NormalMessage() override;
        void topProgress(aFuncore::Inter &inter, aFuncore::Activation &activation) override;
        AFUN_INLINE aFuncore::Object *getObject();

    private:
        aFuncore::Object *obj;
    };

    class AFUN_RT_EXPORT ErrorMessage : public TopMessage {
    public:
        struct TrackBack {
            const aFuntool::FilePath path;
            aFuntool::FileLine line;
        };

        explicit ErrorMessage(std::string error_type_, std::string error_info_, aFuncore::Inter &inter);
        AFUN_INLINE ErrorMessage(ErrorMessage &&msg) noexcept;
        void topProgress(aFuncore::Inter &inter_, aFuncore::Activation &activation) override;
        [[nodiscard]] AFUN_INLINE std::string getErrorType() const;
        [[nodiscard]] AFUN_INLINE std::string getErrorInfo() const;
        [[nodiscard]] AFUN_INLINE const std::list<TrackBack> &getTrackBack() const;

    private:
        aFuncore::Inter &inter;

        std::string error_type;
        std::string error_info;
        std::list<TrackBack> trackback;
    };
}

#include "rt-message.inline.h"
#endif //AFUN_RT_MESSAGE_H
