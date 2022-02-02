#include "aFunrt.h"

using namespace aFunrt;
using namespace aFuncore;
using namespace aFuntool;

void printInterEvent(Inter &inter);

class Func1 : public Function {
    class CallFunc1 : public CallFunction {
        Code &func_code;
        const Code::ByteCode *call_code;
        Inter &inter;
        std::list<ArgCodeList> *acl;
    public:
        CallFunc1(Code &func_code_, const Code::ByteCode *code_, Inter &inter_) : func_code{func_code_}, call_code{code_}, inter{inter_} {
            acl = new std::list<ArgCodeList>;
            if (code_ != nullptr) {
                ArgCodeList agr1 {code_->getSon()->toNext()};
                acl->push_front(agr1);
            }
        }

        std::list<ArgCodeList> *getArgCodeList(Inter &inter_, Activation &activation, const Code::ByteCode *call) override {
            return acl;
        }

        void runFunction() override {
            if (acl->empty())
                printf_stdout(0, "runFunction No AegCodeList\n");
            else
                printf_stdout(0, "runFunction : %p\n", acl->begin()->getObject());
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

    void destruct(Inter &gc_inter) override {
        aFuntool::printf_stdout(0, "%p destruct\n", this);
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_p,
                                                       new Code::ByteCode(code, "test-var", 1), 0));
        gc_inter.runCode(code);
        printInterEvent(gc_inter);
    };
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
        aFuntool::cout << "Literaler1: " << literal  << "prefix: " << (prefix == NUL ? '-' : prefix) << "\n";
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

void thread_test(Inter &son) {
    auto code = Code("run-code.aun");
    code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_p,
                                                   new Code::ByteCode(code, "test-var", 1), 0));
    son.runCode(code);
    printInterEvent(son);
    fputs_stdout("\n");
}

int Main() {
    aFunEnvironment env{};
    Inter inter {env};

    auto obj = new Object("Object", inter);
    inter.getGlobalVarlist()->defineVar("test-var", obj);
    aFuntool::cout << "obj: " << obj << "\n";
    obj->delReference();

    auto func = new Func1(inter);
    inter.getGlobalVarlist()->defineVar("test-func", func);
    aFuntool::cout << "func: " << func << "\n";
    func->delReference();

    auto literaler = new Literaler1(inter);
    inter.getGlobalVarlist()->defineVar("test-literaler", literaler);
    aFuntool::cout << "literaler: " << literaler << "\n";
    literaler->delReference();

    auto cbv = new CBV1(inter);
    inter.getGlobalVarlist()->defineVar("test-cbv", cbv);
    aFuntool::cout << "cbv: " << cbv << "\n";
    inter.getEnvVarSpace().setNumber("sys:error_std", 1);
    cbv->delReference();

    auto tmp = new Func1(inter);
    aFuntool::cout << "tmp: " << tmp << "\n\n";
    tmp->delReference();

    aFuntool::cout << "Checking gc ...\n";
    for (int i = 0; i <= 12; i++) {
        aFuntool::cout << "Wait " << (i * 0.25) << "\n";
        aFuntool::safeSleep(0.25);
    }
    aFuntool::cout << "Check gc finished.\n\n";


    {
        fputs_stdout("Test-1: block-p & get test-var\n");
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_p,
                                                       new Code::ByteCode(code, "test-var", 1), 0));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        fputs_stdout("Test-2: block-c & run {test-func test-var}\n");
        auto code = Code("run-code.aun");

        auto arg = new Code::ByteCode(code, "test-func", 1);
        arg->connect(new Code::ByteCode(code, "test-var", 1));

        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_c, arg, 0));

        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        fputs_stdout("Test-3: block-b & run [test-var test-func]\n");
        auto code = Code("run-code.aun");

        auto arg = new Code::ByteCode(code, "test-var", 1);
        arg->connect(new Code::ByteCode(code, "test-func", 1));

        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_b, arg, 0));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        fputs_stdout("Test-4: test-literaler\n");
        inter.pushLiteral("data[0-9]", "test-literaler", false);
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "data4", 1));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        fputs_stdout("Test-5: test-cbv\n");
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "test-cbv", 1));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        fputs_stdout("Test-6: run-function\n");
        inter.runCode(func);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        fputs_stdout("Test-7: {import test}\n");
        auto code = Code("run-code.aun");

        auto arg = new Code::ByteCode(code, "import", 1);
        arg->connect(new Code::ByteCode(code, "test", 1));

        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_c, arg, 0));

        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        /* 多线程 */
        fputs_stdout("Test-8: thread\n");
        Inter son{inter};
        std::thread thread{thread_test, std::ref(son)};

        {
            auto code = Code("run-code.aun");
            code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_p,
                                                           new Code::ByteCode(code, "test-var", 1), 0));
            inter.runCode(code);
            printInterEvent(inter);
            fputs_stdout("\n");
        }

        thread.join();
    }

    /* 执行错误的代码 */
    {
        fputs_stdout("Test-a: error not var\n");
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, "test-not-var", 1));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        fputs_stdout("Test-last: {exit}\n");
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_c,
                                                       new Code::ByteCode(code, "exit", 1), 0));
        inter.runCode(code);
        printInterEvent(inter);
        fputs_stdout("\n");
    }

    {
        auto code = Code("run-code.aun");
        code.getByteCode()->connect(new Code::ByteCode(code, Code::ByteCode::block_p,
                                                       new Code::ByteCode(code, "test-var", 1), 0));
        inter.runCode(code);
        printInterEvent(inter);
    }

    return 0;
}

int main() {
    int exit_code = 0;
    try {
        std::string base_path = getExedir(1);
        if (base_path.empty()) {
            printf_stderr(0, "aFunlang init error.");
            aFunExitReal(EXIT_FAILURE);
        }

        aFuntool::LogFactory factor{};
        aFuncore::InitInfo info{base_path, factor, true, log_debug};

        if (!aFunCoreInit(&info)) {
            printf_stderr(0, "aFunlang init error.");
            aFunExitReal(EXIT_FAILURE);
        }

        exit_code = Main();
        aFunExit(exit_code);
    } catch (aFuntool::Exit &e) {
        exit_code = e.getExitCode();
    }
    aFunExitReal(exit_code);
}