#ifndef AFUN_RT_TOP_MESSAGE_H
#define AFUN_RT_TOP_MESSAGE_H
#include "aFunRuntimeExport.h"
#include "aFuncore.h"

namespace aFunrt {
    class TopMessage : public virtual aFuncore::Message {
    public:
        virtual void topProgress(aFuncore::Inter &inter, aFuncore::Activation &activation) = 0;
    };
}

#endif //AFUN_RT_TOP_MESSAGE_H
