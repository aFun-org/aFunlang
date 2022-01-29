#include "aFuncore.h"

using namespace aFuncore;
using namespace aFuntool;

class Func1 : public Function {
    class CallFunc1 : public CallFunction {
        Code &func_code;
        const Code::ByteCode *code;
        Inter &inter;
        std::list<ArgCodeList> *acl;
    public:
        CallFunc1(Code &func_code_, const Code::ByteCode *code_, Inter &inter_) : func_code{func_code_}, code{code_}, inter{inter_} {
            acl = new std::list<ArgCodeList>;
            ArgCodeList agr1 = {code_->getSon()->toNext()};
            acl->push_front(agr1);
        }

        std::list<ArgCodeList> *getArgCodeList(Inter &inter_, Activation &activation, const Code::ByteCode *call) override {
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
    explicit Func1(Inter &inter_) : Object("Function", inter_), func_code {"run-code.aun"} {
        func_code.getByteCode()->connect(
                new Code::ByteCode(func_code, Code::ByteCode::block_p,
                                   new Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~Func1() override = default;

    CallFunction *getCallFunction(const Code::ByteCode *code, Inter &inter) override {
        return dynamic_cast<CallFunction *>(new CallFunc1(func_code, code, inter));
    }

    bool isInfix() override {return true;}
};

class Literaler1 : public Literaler {
    Code func_code;
public:
    explicit Literaler1(Inter &inter_) : Object("Data", inter_), func_code{"run-code.aun"} {
        func_code.getByteCode()->connect(
                new Code::ByteCode(func_code, Code::ByteCode::block_p,
                                   new Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~Literaler1() override = default;

    void getObject(const std::string &literal, char prefix, Inter &inter, Activation &activation) override {
        aFuntool::cout << "Literaler1: " << literal << (prefix == NUL ? '-' : prefix) << "\n";
        new ExeActivation(func_code, inter);
    }
};

class CBV1 : public CallBackVar {
    Code func_code;
public:
    explicit CBV1(Inter &inter_) : Object("CBV1", inter_), func_code{"run-code.aun"} {
        func_code.getByteCode()->connect(
                new Code::ByteCode(func_code, Code::ByteCode::block_p,
                                   new Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~CBV1() override = default;

    void callBack(Inter &inter, Activation &activation) override {
        aFuntool::cout << "CallBackVar callback\n";
        new ExeActivation(func_code, inter);
    }
};

static void printMessage(const std::string &type, Message *msg, Inter &inter) {
    if (type == "NORMAL") {
        auto *msg_ = dynamic_cast<NormalMessage *>(msg);
        if (msg_ == nullptr)
            return;
        aFuntool::printf_stdout(0, "NORMAL: %p\n", msg_->getObject());
    } else if (type == "ERROR") {
        auto *msg_ = dynamic_cast<ErrorMessage *>(msg);
        if (msg_ == nullptr)
            return;
        int32_t error_std = 0;
        inter.getEnvVarSpace().findNumber("sys:error_std", error_std);
        if (error_std == 0) {
            aFuntool::printf_stderr(0, "Error TrackBack\n");
            for (auto &begin: msg_->getTrackBack())
                aFuntool::printf_stderr(0, "  File \"%s\", line %d\n", begin.path.c_str(), begin.line);
            aFuntool::printf_stderr(0, "%s: %s\n", msg_->getErrorType().c_str(), msg_->getErrorInfo().c_str());
        } else {
            aFuntool::printf_stdout(0, "Error TrackBack\n");
            for (auto &begin: msg_->getTrackBack())
                aFuntool::printf_stdout(0, "  File \"%s\", line %d\n", begin.path.c_str(), begin.line);
            aFuntool::printf_stdout(0, "%s: %s\n", msg_->getErrorType().c_str(), msg_->getErrorInfo().c_str());
        }
    }
}

void printInterEvent(Inter &inter) {
    std::string type;
    for (auto msg = inter.getOutMessageStream().popFrontMessage(type); msg != nullptr; msg = inter.getOutMessageStream().popFrontMessage(type)) {
        printMessage(type, msg, inter);
        delete msg;
    }
}

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
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        auto code = Code("run-code.aun");

        auto arg = new Code::ByteCode(code, "test-func", 1);
        arg->connect(new Code::ByteCode(code, "test-var", 1));

        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_c, arg, 0));

        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        auto code = Code("run-code.aun");

        auto arg = new Code::ByteCode(code, "test-var", 1);
        arg->connect(new Code::ByteCode(code, "test-func", 1));

        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_b, arg, 0));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        inter.pushLiteral("data[0-9]", "test-literaler", false);
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "data3", 1));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "test-cbv", 1));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    /* 执行错误的代码 */
    {
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "test-not-var", 1));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    /* 多线程 */
    {
        Inter son {inter};
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "test-not-var", 1));
        son.runCode(code);
        printInterEvent(son);
        fputs_stdout("\n");
    }

    /* 不会执行的代码 */
    inter.setInterExit();

    {
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_p,
                                                       new Code::ByteCode(code, "test-var", 1), 0));
        inter.runCode(code);
        printInterEvent(inter);
    }

    return 0;
}