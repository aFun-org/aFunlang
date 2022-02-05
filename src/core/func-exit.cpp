#include "func-exit.h"

namespace aFuncore {
    Function::CallFunction *ExitFunction::getCallFunction(const Code::ByteCode *code, Inter &inter) {
        return dynamic_cast<CallFunction *>(new CallFunc(code, inter));
    }

    ExitFunction::CallFunc::CallFunc(const Code::ByteCode *code_, Inter &inter_) : call_code{code_}, inter{inter_} {
        acl = new std::list<ArgCodeList>;
        if (code_ != nullptr) {
            auto arg_code = code_->getSon()->toNext();
            if (arg_code != nullptr) {
                ArgCodeList agr1{code_->getSon()->toNext()};
                acl->push_front(agr1);
            }
        }
    }

    std::list<Function::CallFunction::ArgCodeList> *ExitFunction::CallFunc::getArgCodeList(Inter &inter_,
                                                                                                      Activation &activation,
                                                                                                     const Code::ByteCode *call) {
        return acl;
    }

    void ExitFunction::CallFunc::runFunction() {
        inter.setInterExit();
        auto &stream = inter.getActivation()->getDownStream();
        if (acl->empty()) {
            auto none = new Object("None", inter);
            stream.pushMessage("NORMAL", new NormalMessage(none));
            none->delReference();
        } else
            stream.pushMessage("NORMAL", new NormalMessage(acl->begin()->getObject()));

    }

    ExitFunction::CallFunc::~CallFunc() {
        delete acl;
    }
}