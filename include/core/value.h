#ifndef AFUN_VALUE_H
#define AFUN_VALUE_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "list"
#include "core.h"
#include "gc.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Object : public GcObject<class Object> {
    public:
        Inter *const inter;
        const std::string type;  // 标识 Object 的字符串

        explicit Object(const std::string &type_, Inter *inter_);
        ~Object() override =default;
    };

    class AFUN_CORE_EXPORT Function : public Object {
    public:
        Function(const std::string &type_, Inter *inter_) : Object(type_ + ":Function", inter_) {}

        class AFUN_CORE_EXPORT CallFunction {
        public:
            struct ArgCodeList {
                Code *code = nullptr;
                Object *ret = nullptr;
            };
            CallFunction() = default;
            virtual ~CallFunction() = default;
            CallFunction(const CallFunction &)=delete;
            CallFunction &operator=(const CallFunction &)=delete;

            virtual std::list<ArgCodeList> *getArgCodeList() = 0;
            virtual void runFunction() = 0;
        };
        virtual CallFunction *getCallFunction(Code *code, Inter *inter) = 0;
        virtual bool isInfix() {return false;}
    };

    class AFUN_CORE_EXPORT Literaler : public Object {
    public:
        Literaler(const std::string &type_, Inter *inter_) : Object(type_ + ":Literaler", inter_) {}

        virtual void getObject(const std::string &literal, char prefix) = 0;
    };

    class AFUN_CORE_EXPORT CallBackVar : public Object {
    public:
        CallBackVar(const std::string &type_, Inter *inter_) : Object(type_ + ":CallBackVar", inter_) {}

        virtual bool isCallBack() {return true;}
        virtual void callBack() = 0;
    };
};

#endif //AFUN_VALUE_H
