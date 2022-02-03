#ifndef AFUN_FUNC_IMPORT_H
#define AFUN_FUNC_IMPORT_H
#include "aFunlangExport.h"
#include "aFuncore.h"

namespace aFunit {
    class ImportFunction : public aFuncore::Function {
        class CallFunc : public CallFunction {
            const aFuncore::Code::ByteCode *call_code;
            aFuncore::Inter &inter;
            std::list<ArgCodeList> *acl;
            std::string import;
        public:
            CallFunc(const aFuncore::Code::ByteCode *code_, aFuncore::Inter &inter_);
            std::list<ArgCodeList> *getArgCodeList(aFuncore::Inter &inter_,
                                                   aFuncore:: Activation &activation,
                                                   const aFuncore::Code::ByteCode *call) override;

            void runFunction() override;
            ~CallFunc() override;
        };

    public:
        inline explicit ImportFunction(aFuncore::Inter &inter_);
        inline explicit ImportFunction(aFuncore::Environment &env_);
        ~ImportFunction() override = default;

        CallFunction *getCallFunction(const aFuncore::Code::ByteCode *code, aFuncore::Inter &inter) override;
    };
}

#include "func-import.inline.h"

#endif //AFUN_FUNC_IMPORT_H
