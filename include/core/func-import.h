#ifndef AFUN_FUNC_IMPORT_H
#define AFUN_FUNC_IMPORT_H
#include "aFunlangExport.h"
#include "aFuncore.h"

namespace aFuncore {
    class ImportFunction : public Function {
        class CallFunc : public CallFunction {
            const Code::ByteCode *call_code;
            Inter &inter;
            std::list<ArgCodeList> *acl;
            std::string import;
        public:
            CallFunc(const Code::ByteCode *code_, Inter &inter_);
            std::list<ArgCodeList> *getArgCodeList(Inter &inter_,
                                                    Activation &activation,
                                                   const Code::ByteCode *call) override;

            void runFunction() override;
            ~CallFunc() override;
        };

    public:
        inline explicit ImportFunction(Inter &inter_);
        inline explicit ImportFunction(Environment &env_);
        ~ImportFunction() override = default;

        CallFunction *getCallFunction(const Code::ByteCode *code, Inter &inter) override;
    };
}

#include "func-import.inline.h"

#endif //AFUN_FUNC_IMPORT_H
