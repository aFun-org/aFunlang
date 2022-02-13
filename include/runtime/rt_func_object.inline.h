#ifndef AFUN_RT_FUNC_OBJECT_INLINE_H
#define AFUN_RT_FUNC_OBJECT_INLINE_H
#include "rt_func_object.h"

namespace aFunrt {
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


#endif //AFUN_RT_FUNC_OBJECT_INLINE_H
