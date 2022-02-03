#ifndef AFUN_FUNC_EXIT_H
#define AFUN_FUNC_EXIT_H
#include "aFunlangExport.h"
#include "aFuncore.h"

namespace aFunit {
    class ExitFunction : public aFuncore::Function {
        class CallFunc : public CallFunction {
            const aFuncore::Code::ByteCode *call_code;
            aFuncore::Inter &inter;
            std::list<ArgCodeList> *acl;
        public:
            CallFunc(const aFuncore::Code::ByteCode *code_, aFuncore::Inter &inter_);
            std::list<ArgCodeList> *getArgCodeList(aFuncore::Inter &inter_,
                                                   aFuncore:: Activation &activation,
                                                   const aFuncore::Code::ByteCode *call) override;

            void runFunction() override;
            ~CallFunc() override;
        };

    public:
        inline explicit ExitFunction(aFuncore::Inter &inter_);
        inline explicit ExitFunction(aFuncore::Environment &env_);
        ~ExitFunction() override = default;

        CallFunction *getCallFunction(const aFuncore::Code::ByteCode *code, aFuncore::Inter &inter) override;
    };
}

#include "func-exit.inline.h"

#endif //AFUN_FUNC_EXIT_H
