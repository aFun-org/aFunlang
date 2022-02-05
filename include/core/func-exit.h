#ifndef AFUN_FUNC_EXIT_H
#define AFUN_FUNC_EXIT_H
#include "aFunlangExport.h"
#include "aFuncore.h"

namespace aFuncore {
    class ExitFunction : public Function {
        class CallFunc : public CallFunction {
            const Code::ByteCode *call_code;
            Inter &inter;
            std::list<ArgCodeList> *acl;
        public:
            CallFunc(const Code::ByteCode *code_, Inter &inter_);
            std::list<ArgCodeList> *getArgCodeList(Inter &inter_,
                                                    Activation &activation,
                                                   const Code::ByteCode *call) override;

            void runFunction() override;
            ~CallFunc() override;
        };

    public:
        inline explicit ExitFunction(Inter &inter_);
        inline explicit ExitFunction(Environment &env_);
        ~ExitFunction() override = default;

        CallFunction *getCallFunction(const Code::ByteCode *code, Inter &inter) override;
    };
}

#include "func-exit.inline.h"

#endif //AFUN_FUNC_EXIT_H
