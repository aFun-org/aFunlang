#include "aFuncore.h"

using namespace aFuncore;
using namespace aFuntool;

class Func1 : public Function {
    class CallFunc1 : public CallFunction {
        Code &func_code;
        Code::ByteCode *code;
        Inter &inter;
        std::list<ArgCodeList> *acl;
    public:
        CallFunc1(Code &func_code_, Code::ByteCode *code_, Inter &inter_) : func_code{func_code_}, code{code_}, inter{inter_} {
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

    Code func_code;
public:
    explicit Func1(Inter &inter_) : Function("Function", inter_), func_code {"run-code.aun"} {
        func_code.getByteCode()->connect(
                new Code::ByteCode(func_code, Code::ByteCode::block_p,
                                   new Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~Func1() override = default;

    CallFunction *getCallFunction(Code::ByteCode *code, Inter &inter) override {
        return dynamic_cast<CallFunction *>(new CallFunc1(func_code, code, inter));
    }

    bool isInfix() override {return true;}
};

class Literaler1 : public Literaler {
    Code func_code;
public:
    explicit Literaler1(Inter &inter_) : Literaler("Data", inter_), func_code{"run-code.aun"} {
        func_code.getByteCode()->connect(
                new Code::ByteCode(func_code, Code::ByteCode::block_p,
                                   new Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~Literaler1() override = default;

    void getObject(const std::string &literal, char prefix, Inter &inter) override {
        aFuntool::cout << "Literaler1: " << literal << (prefix == NUL ? '-' : prefix) << "\n";
        new ExeActivation(func_code, inter);
    }
};

class CBV1 : public CallBackVar {
    Code func_code;
public:
    explicit CBV1(Inter &inter_) : CallBackVar("CBV1", inter_), func_code{"run-code.aun"} {
        func_code.getByteCode()->connect(
                new Code::ByteCode(func_code, Code::ByteCode::block_p,
                                   new Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~CBV1() override = default;

    void callBack(Inter &inter) override {
        aFuntool::cout << "CallBackVar callback\n";
        new ExeActivation(func_code, inter);
    }
};

int main() {
    Environment env {};
    Inter inter {env};

    auto obj = new Object("Object", inter);
    inter.getGlobalVarlist()->defineVar("test-var", obj);
    aFuntool::cout << "obj: " << obj << "\n";

    auto func = new Func1(inter);
    inter.getGlobalVarlist()->defineVar("test-func", func);
    aFuntool::cout << "func: " << func << "\n";

    auto literaler = new Literaler1(inter);
    inter.getGlobalVarlist()->defineVar("test-literaler", literaler);
    aFuntool::cout << "literaler: " << literaler << "\n";

    auto cbv = new CBV1(inter);
    inter.getGlobalVarlist()->defineVar("test-cbv", cbv);
    aFuntool::cout << "cbv: " << cbv << "\n\n";
    inter.getEnvVarSpace().setNumber("sys:error_std", 1);

    {
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_p,
                                                       new Code::ByteCode(code, "test-var", 1), 0));
        inter.runCode(code);
        fputs_stdout("\n");
    }

    {
        auto code = Code("run-code.aun");

        auto arg = new Code::ByteCode(code, "test-func", 1);
        arg->connect(new Code::ByteCode(code, "test-var", 1));

        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_c, arg, 0));

        inter.runCode(code);
        fputs_stdout("\n");
    }

    {
        auto code = Code("run-code.aun");

        auto arg = new Code::ByteCode(code, "test-var", 1);
        arg->connect(new Code::ByteCode(code, "test-func", 1));

        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_b, arg, 0));
        inter.runCode(code);
        fputs_stdout("\n");
    }

    {
        inter.pushLiteral("data[0-9]", "test-literaler", false);
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "data3", 1));
        inter.runCode(code);
        fputs_stdout("\n");
    }

    {
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "test-cbv", 1));
        inter.runCode(code);
        fputs_stdout("\n");
    }

    {
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "test-not-var", 1));
        inter.runCode(code);
        fputs_stdout("\n");
    }

    {
        Inter son {inter};
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "test-not-var", 1));
        son.runCode(code);
        fputs_stdout("\n");
    }

    return 0;
}