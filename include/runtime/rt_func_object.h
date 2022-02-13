#ifndef AFUN_RT_FUNC_OBJECT_H
#define AFUN_RT_FUNC_OBJECT_H
#include <list>
#include "aFunRuntimeExport.h"
#include "aFuncore.h"

namespace aFunrt {
    class AFUN_RT_EXPORT Function : public virtual aFuncore::Object {
    public:
        class AFUN_RT_EXPORT CallFunction;

        virtual CallFunction *getCallFunction(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter) = 0;
        virtual bool isInfix();
    };

    class AFUN_RT_EXPORT Function::CallFunction {
    public:
        class ArgCodeList;

        CallFunction() = default;
        virtual ~CallFunction() = default;
        CallFunction(const CallFunction &)=delete;
        CallFunction &operator=(const CallFunction &)=delete;

        virtual std::list<ArgCodeList> *getArgCodeList(aFuncore::Inter &inter, aFuncore::Activation &activation, const aFuncode::Code::ByteCode *call) = 0;
        virtual void runFunction() = 0;
    };

    class Function::CallFunction::ArgCodeList {
    public:
        const aFuncode::Code::ByteCode *code = nullptr;
        AFUN_INLINE explicit ArgCodeList(const aFuncode::Code::ByteCode *code = nullptr);
        AFUN_INLINE ~ArgCodeList();
        AFUN_INLINE aFuncore::Object *setObject(aFuncore::Object *res);
        AFUN_INLINE aFuncore::Object *getObject();
    private:
        aFuncore::Object *ret;
    };
}

#include "rt_func_object.inline.h"

#endif //AFUN_RT_FUNC_OBJECT_H
