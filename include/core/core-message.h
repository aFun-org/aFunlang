#ifndef AFUN_CORE_MESSAGE_H
#define AFUN_CORE_MESSAGE_H
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Message {
    public:
        AFUN_INLINE explicit Message() = default;
        virtual ~Message() = default;
        Message &operator=(const Message &)=delete;
    };
}

#endif //AFUN_CORE_MESSAGE_H
