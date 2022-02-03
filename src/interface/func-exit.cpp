#include "func-exit.h"

namespace aFunit {
    aFuncore::Function::CallFunction *ExitFunction::getCallFunction(const aFuncore::Code::ByteCode *code, aFuncore::Inter &inter) {
        return dynamic_cast<CallFunction *>(new CallFunc(code, inter));
    }

    ExitFunction::CallFunc::CallFunc(const aFuncore::Code::ByteCode *code_, aFuncore::Inter &inter_) : call_code{code_}, inter{inter_} {
        acl = new std::list<ArgCodeList>;
        if (code_ != nullptr) {
            auto arg_code = code_->getSon()->toNext();
            if (arg_code != nullptr) {
                ArgCodeList agr1{code_->getSon()->toNext()};
                acl->push_front(agr1);
            }
        }
    }

    std::list<aFuncore::Function::CallFunction::ArgCodeList> *ExitFunction::CallFunc::getArgCodeList(aFuncore::Inter &inter_,
                                                                                                     aFuncore:: Activation &activation,
                                                                                                     const aFuncore::Code::ByteCode *call) {
        return acl;
    }

    void ExitFunction::CallFunc::runFunction() {
        inter.setInterExit();
        auto &stream = inter.getActivation()->getDownStream();
        if (acl->empty()) {
            auto none = new aFuncore::Object("None", inter);
            stream.pushMessage("NORMAL", new aFuncore::NormalMessage(none));
            none->delReference();
        } else
            stream.pushMessage("NORMAL", new aFuncore::NormalMessage(acl->begin()->getObject()));

    }

    ExitFunction::CallFunc::~CallFunc() {
        delete acl;
    }
}