﻿#ifndef AFUN_VALUE_H
#define AFUN_VALUE_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "list"
#include "gc.h"
#include "code.h"

namespace aFuncore {
    class Inter;

    class AFUN_CORE_EXPORT Object : public GcObject<class Object> {
    public:
        Inter &inter;
        const std::string type;  // 标识 Object 的字符串

        explicit Object(const std::string &type_, Inter &inter_);
        ~Object() override = default;
    };

    class AFUN_CORE_EXPORT Function : public Object {
    public:
        class AFUN_CORE_EXPORT CallFunction;

        inline Function(const std::string &type_, Inter &inter_);
        virtual CallFunction *getCallFunction(Code *code, Inter &inter) = 0;
        virtual inline bool isInfix();
    };

    class AFUN_CORE_EXPORT Function::CallFunction {
    public:
        struct ArgCodeList;

        CallFunction() = default;
        virtual ~CallFunction() = default;
        CallFunction(const CallFunction &)=delete;
        CallFunction &operator=(const CallFunction &)=delete;

        virtual std::list<ArgCodeList> *getArgCodeList() = 0;
        virtual void runFunction() = 0;
    };

    struct Function::CallFunction::ArgCodeList {
        Code *code = nullptr;
        Object *ret = nullptr;
    };

    class AFUN_CORE_EXPORT Literaler : public Object {
    public:
        inline Literaler(const std::string &type_, Inter &inter_);
        virtual void getObject(const std::string &literal, char prefix) = 0;
    };

    class AFUN_CORE_EXPORT CallBackVar : public Object {
    public:
        inline CallBackVar(const std::string &type_, Inter &inter_);
        virtual inline bool isCallBack();
        virtual void callBack() = 0;
    };
};

#include "value.inline.h"

#endif //AFUN_VALUE_H
