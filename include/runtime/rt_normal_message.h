#ifndef AFUN_RT_NORMAL_MESSAGE_H
#define AFUN_RT_NORMAL_MESSAGE_H
#include "aFunRuntimeExport.h"
#include "aFuncore.h"
#include "rt_top_message.h"

namespace aFunrt {
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
}

#include "rt_normal_message.inline.h"
#endif //AFUN_RT_NORMAL_MESSAGE_H
