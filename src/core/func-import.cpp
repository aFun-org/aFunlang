#include "func-import.h"

namespace aFuncore {
    Function::CallFunction *ImportFunction::getCallFunction(const Code::ByteCode *code, Inter &inter) {
        return dynamic_cast<CallFunction *>(new CallFunc(code, inter));
    }

    ImportFunction::CallFunc::CallFunc(const Code::ByteCode *code_, Inter &inter_) : call_code{code_}, inter{inter_} {
        if (code_ == nullptr ||
            code_->getSon() == nullptr ||
            code_->getSon()->toNext() == nullptr ||
            code_->getSon()->toNext()->getType() != Code::ByteCode::code_element)
            throw ArgumentError();
        acl = new std::list<ArgCodeList>;
        import = code_->getSon()->toNext()->getElement();
    }

    std::list<Function::CallFunction::ArgCodeList> *ImportFunction::CallFunc::getArgCodeList(Inter &inter_,
                                                                                                        Activation &activation,
                                                                                                       const Code::ByteCode *call) {
        return acl;
    }

    void ImportFunction::CallFunc::runFunction() {
        auto &stream = inter.getActivation()->getDownStream();
        auto none = new Object("None", inter);
        stream.pushMessage("NORMAL", new NormalMessage(none));
        none->delReference();
        aFuntool::cout << "Import " << import << "\n";
    }

    ImportFunction::CallFunc::~CallFunc() {
        delete acl;
    }
}
