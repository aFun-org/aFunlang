#ifndef AFUN_VALUE_HPP
#define AFUN_VALUE_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"
#include "list"
#include "core.hpp"
#include "gc.hpp"

namespace aFuncore {
    class Object : public GcObject<class Object> {
    public:
        Inter *const inter;
        const std::string type;  // 标识 Object 的字符串

        AFUN_CORE_EXPORT explicit Object(const std::string &type_, Inter *inter_);
        AFUN_CORE_EXPORT ~Object() override =default;
    };

    class Function : public Object {
    public:
        Function(const std::string &type_, Inter *inter_) : Object(type_, inter_) {}
        class CallFunction {
        public:
            struct ArgCodeList {
                Code *code = nullptr;
                Object *ret = nullptr;
            };
            virtual ~CallFunction()=default;
            virtual std::list<ArgCodeList> *getArgCodeList()=0;
            virtual ActivationStatus runFunction()=0;
        };
        virtual CallFunction *getCallFunction(Code *code, Inter *inter)=0;
        virtual bool isInfix() {return false;}
    };
};

#endif //AFUN_VALUE_HPP
