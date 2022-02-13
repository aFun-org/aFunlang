#ifndef AFUN_RT_OBJECT_INLINE_H
#define AFUN_RT_OBJECT_INLINE_H
#include "rt-object.h"

namespace aFunrt {
    size_t VarSpace::getCount() {
        std::unique_lock<std::mutex> mutex{lock};
        return var.size();
    }

    aFuncore::Object *VarSpace::findObject(const std::string &name) {
        Var *ret = findVar(name);
        return ret ? ret->getData() : nullptr;
    }

    Function::CallFunction::ArgCodeList::ArgCodeList(const aFuncode::Code::ByteCode *code_) : code{code_}, ret{nullptr} {

    }

    Function::CallFunction::ArgCodeList::~ArgCodeList() {
        if (ret != nullptr)
            ret->delReference();
    }

    aFuncore::Object *Function::CallFunction::ArgCodeList::setObject(Object *res) {
        Object *obj = ret;
        ret = res;
        if (ret != nullptr)
            ret->addReference();
        return obj;
    }

    aFuncore::Object *Function::CallFunction::ArgCodeList::getObject() {
        return ret;
    }
};


#endif //AFUN_RT_OBJECT_INLINE_H
