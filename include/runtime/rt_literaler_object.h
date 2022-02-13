#ifndef AFUN_RT_LITERALER_OBJECT_H
#define AFUN_RT_LITERALER_OBJECT_H
#include <list>
#include "aFunRuntimeExport.h"
#include "aFuncore.h"

namespace aFunrt {
    class AFUN_RT_EXPORT Literaler : public virtual aFuncore::Object {
    public:
        virtual void getObject(const std::string &literal, char prefix, aFuncore::Inter &inter, aFuncore::Activation &activation) = 0;
    };
}

#endif //AFUN_RT_LITERALER_OBJECT_H
