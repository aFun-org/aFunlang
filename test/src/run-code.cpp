#include "inter.hpp"
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

        void runFunction() override {
            printf_stdout(0, "runFunction : %p\n", acl->begin()->ret);
            new ExeActivation(func_code, inter);
        }

        ~CallFunc1() override {
            delete acl;
        }
    };

    Code *func_code;
public:
    explicit Func1(Inter *inter_) : Function("Function", inter_) {
        func_code = (Code::create(0, "run-code.aun"));
        func_code->connect(Code::create(block_p, Code::create("test-var", 1), 0));
    }

    ~Func1() override {
        Code::destruct(func_code);
    }

    CallFunction *getCallFunction(Code *code, Inter *inter) override {
        return dynamic_cast<CallFunction *>(new CallFunc1(func_code, code, inter));
    }

    bool isInfix() override {return true;}
};

class Literaler1 : public Literaler {
    Code *func_code;
public:
    explicit Literaler1(Inter *inter_) : Literaler("Data", inter_) {
        func_code = (Code::create(0, "run-code.aun"));
        func_code->connect(Code::create(block_p, Code::create("test-var", 1), 0));
    }

    ~Literaler1() override {
        Code::destruct(func_code);
    }

    void getObject(const std::string &literal, char prefix) override {
        printf_stdout(0, "Literaler1: %s %c\n", literal.c_str(), prefix == NUL ? '-' : prefix);
        new ExeActivation(func_code, inter);
    }
};

class CBV1 : public CallBackVar {
    Code *func_code;
public:
    explicit CBV1(Inter *inter_) : CallBackVar("CBV1", inter_) {
        func_code = (Code::create(0, "run-code.aun"));
        func_code->connect(Code::create(block_p, Code::create("test-var", 1), 0));
    }

    ~CBV1() override {
        Code::destruct(func_code);
    }

    void callBack() override {
        printf_stdout(0, "CallBackVar callback\n");
        new ExeActivation(func_code, inter);
    }
};

int main() {
    auto inter = new Inter();

    auto obj = new Object("Object", inter);
    inter->getGlobalVarlist()->defineVar("test-var", obj);
    printf_stdout(0, "obj: %p\n", obj);

    auto func = new Func1(inter);
    inter->getGlobalVarlist()->defineVar("test-func", func);
    printf_stdout(0, "func: %p\n", func);

    auto literaler = new Literaler1(inter);
    inter->getGlobalVarlist()->defineVar("test-literaler", literaler);
    printf_stdout(0, "literaler: %p\n", literaler);

    auto cbv = new CBV1(inter);
    inter->getGlobalVarlist()->defineVar("test-cbv", cbv);
    printf_stdout(0, "cbv: %p\n", cbv);
    fputs_stdout("\n");

    {
        auto code = (Code::create(0, "run-code.aun"));
        code->connect(Code::create(block_p, Code::create("test-var", 1), 0));
        inter->runCode(code);
        Code::destruct(code);
        fputs_stdout("\n");
    }

    {
        auto arg = Code::create("test-func", 1);
        arg->connect(Code::create("test-var", 1));

        auto code = (Code::create(0, "run-code.aun"));
        code->connect(Code::create(block_c, arg, 0));
        inter->runCode(code);
        Code::destruct(code);
        fputs_stdout("\n");
    }

    {
        auto arg = Code::create("test-var", 1);
        arg->connect(Code::create("test-func", 1));

        auto code = (Code::create(0, "run-code.aun"));
        code->connect(Code::create(block_b, arg, 0));
        inter->runCode(code);
        Code::destruct(code);
        fputs_stdout("\n");
    }

    {
        inter->pushLiteral("data[0-9]", "test-literaler", false);
        auto code = (Code::create(0, "run-code.aun"));
        code->connect(Code::create("data3", 1));
        inter->runCode(code);
        Code::destruct(code);
        fputs_stdout("\n");
    }

    {
        auto code = (Code::create(0, "run-code.aun"));
        code->connect(Code::create("test-cbv", 1));
        inter->runCode(code);
        Code::destruct(code);
        fputs_stdout("\n");
    }

    {
        auto code = (Code::create(0, "run-code.aun"));
        code->connect(Code::create("test-not-var", 1));
        inter->runCode(code);
        Code::destruct(code);
        fputs_stdout("\n");
    }

    delete inter;
    return 0;
}