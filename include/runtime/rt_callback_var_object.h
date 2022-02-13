#ifndef AFUN_RT_CALLBACK_VAR_OBJECT_H
#define AFUN_RT_CALLBACK_VAR_OBJECT_H
#include <list>
#include "aFunRuntimeExport.h"
#include "aFuncore.h"

namespace aFunrt {
    class AFUN_RT_EXPORT CallBackVar : public virtual aFuncore::Object {
    public:
        virtual bool isCallBack(aFuncore::Inter &inter, aFuncore::Activation &activation);
        virtual void callBack(aFuncore::Inter &inter, aFuncore::Activation &activation) = 0;
    };
}

#endif //AFUN_RT_CALLBACK_VAR_OBJECT_H
