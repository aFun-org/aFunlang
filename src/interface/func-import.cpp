#include "func-import.h"

namespace aFunit {
    aFuncore::Function::CallFunction *ImportFunction::getCallFunction(const aFuncore::Code::ByteCode *code, aFuncore::Inter &inter) {
        return dynamic_cast<CallFunction *>(new CallFunc(code, inter));
    }

    ImportFunction::CallFunc::CallFunc(const aFuncore::Code::ByteCode *code_, aFuncore::Inter &inter_) : call_code{code_}, inter{inter_} {
        if (code_ == nullptr ||
            code_->getSon() == nullptr ||
            code_->getSon()->toNext() == nullptr ||
            code_->getSon()->toNext()->getType() != aFuncore::Code::ByteCode::code_element)
            throw aFuncore::ArgumentError();
        acl = new std::list<ArgCodeList>;
        import = code_->getSon()->toNext()->getElement();
    }

    std::list<aFuncore::Function::CallFunction::ArgCodeList> *ImportFunction::CallFunc::getArgCodeList(aFuncore::Inter &inter_,
                                                                                                       aFuncore:: Activation &activation,
                                                                                                       const aFuncore::Code::ByteCode *call) {
        return acl;
    }

    void ImportFunction::CallFunc::runFunction() {
        auto &stream = inter.getActivation()->getDownStream();
        auto none = new aFuncore::Object("None", inter);
        stream.pushMessage("NORMAL", new aFuncore::NormalMessage(none));
        none->delReference();
        aFuntool::cout << "Import " << import << "\n";
    }

    ImportFunction::CallFunc::~CallFunc() {
        delete acl;
    }
}
