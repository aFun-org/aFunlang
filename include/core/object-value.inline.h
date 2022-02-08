﻿#ifndef AFUN_OBJECT_VALUE_INLINE_H
#define AFUN_OBJECT_VALUE_INLINE_H
#include "object-value.h"

namespace aFuncore {
    size_t VarSpace::getCount() {
        std::unique_lock<std::mutex> mutex{lock};
        return var.size();
    }

    Object *VarSpace::findObject(const std::string &name) {
        Var *ret = findVar(name);
        return ret ? ret->getData() : nullptr;
    }

    ProtectVarSpace::ProtectVarSpace(Inter &inter) : VarSpace(inter), is_protect{false} {

    }

    ProtectVarSpace::ProtectVarSpace(Environment &env_) : VarSpace(env_), is_protect{false} {

    }

    bool ProtectVarSpace::getProtect() const {
        return is_protect;
    }

    bool ProtectVarSpace::setProtect(bool protect) {
        bool ret = is_protect; is_protect = protect; return ret;
    }

    Function::CallFunction::ArgCodeList::ArgCodeList(const Code::ByteCode *code_) : code{code_}, ret{nullptr} {

    }

    Function::CallFunction::ArgCodeList::~ArgCodeList() {
        if (ret != nullptr)
            ret->delReference();
    }

    Object *Function::CallFunction::ArgCodeList::setObject(Object *res) {
        Object *obj = ret;
        ret = res;
        if (ret != nullptr)
            ret->addReference();
        return obj;
    }

    Object *Function::CallFunction::ArgCodeList::getObject() {
        return ret;
    }

    ImportFunction::ImportFunction(Inter &inter_) : Object("Function", inter_) {

    }

    ImportFunction::ImportFunction(Environment &env_) : Object("Function", env_) {

    }
};

#endif //AFUN_OBJECT_VALUE_INLINE_H
