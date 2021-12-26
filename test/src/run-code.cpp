﻿#include "inter.hpp"
#include "value.hpp"
#include "var.hpp"
#include "code.hpp"
#include "msg.hpp"
#include "activation.hpp"

using namespace aFuncore;
using namespace aFuntool;

class Func1 : public Function {
    class CallFunc1 : public CallFunction {
        Code *func_code;
        Code *code;
        Inter *inter;
        std::list<ArgCodeList> *acl;
    public:
        CallFunc1(Code *func_code_, Code *code_, Inter *inter_) : func_code{func_code_}, code{code_}, inter{inter_} {
            acl = new std::list<ArgCodeList>;
            ArgCodeList agr1 = {code_->getSon()->toNext()};
            acl->push_front(agr1);
        }

        std::list<ArgCodeList> *getArgCodeList() override {
            return acl;
        }

        ActivationStatus runFunction() override {
            printf_stdout(0, "runFunction : %p\n", acl->begin()->ret);
            new ExeActivation(func_code, inter);
            return aFuncore::as_run;
        }

        ~CallFunc1() override {
            delete acl;
        }
    };

    Code *func_code;
public:
    explicit Func1(Inter *inter_) : Function("Function", inter_) {
        func_code = (new Code(0, "run-code.aun"));
        func_code->connect(new Code(block_p, new Code("test-var", 1), 0));
    }

    ~Func1() override {
        func_code->destructAll();
    }

    CallFunction *getCallFunction(Code *code, Inter *inter) override {
        return dynamic_cast<CallFunction *>(new CallFunc1(func_code, code, inter));
    }

    bool isInfix() override {return true;}
};

int main() {
    auto *inter = new Inter();
    auto *obj = new Object("Object", inter);
    inter->getGlobalVarlist()->defineVar("test-var", obj);
    printf_stdout(0, "obj: %p\n", obj);

    auto func = new Func1(inter);
    inter->getGlobalVarlist()->defineVar("test-func", func);
    printf_stdout(0, "func: %p\n", func);

    {
        auto code = (new Code(0, "run-code.aun"));
        code->connect(new Code(block_p, new Code("test-var", 1), 0));
        inter->runCode(code);
        code->destructAll();
    }

    {
        auto arg = new Code("test-func", 1);
        arg->connect(new Code("test-var", 1));

        auto code = (new Code(0, "run-code.aun"));
        code->connect(new Code(block_c, arg, 0));
        inter->runCode(code);
        code->destructAll();
    }

    {
        auto arg = new Code("test-var", 1);
        arg->connect(new Code("test-func", 1));

        auto code = (new Code(0, "run-code.aun"));
        code->connect(new Code(block_b, arg, 0));
        inter->runCode(code);
        code->destructAll();
    }

    delete inter;
    return 0;
}