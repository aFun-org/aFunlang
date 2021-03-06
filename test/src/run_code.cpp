#include "aFunit.h"
#include "aFunrt.h"

void printMessage(const std::string &type, aFuncore::Message *msg, aFuncore::Inter &) {
    if (type == "NORMAL") {
        auto *msg_ = dynamic_cast<aFunrt::NormalMessage *>(msg);
        if (msg_ == nullptr)
            return;
        aFuntool::printf_stdout(0, "NORMAL: %p\n", msg_->getObject());
    } else if (type == "ERROR") {
        auto *msg_ = dynamic_cast<aFunrt::ErrorMessage *>(msg);
        if (msg_ == nullptr)
            return;
        aFuntool::printf_stdout(0, "Error TrackBack\n");
        for (auto &begin: msg_->getTrackBack())
            aFuntool::printf_stdout(0, "  File \"%s\", line %d\n", begin.path.c_str(), begin.line);
        aFuntool::printf_stdout(0, "%s: %s\n", msg_->getErrorType().c_str(), msg_->getErrorInfo().c_str());
    }
}

void progressInterEvent(aFuncore::Inter &inter) {
    std::string type;
    for (auto msg = inter.getOutMessageStream().popFrontMessage(type); msg != nullptr; msg = inter.getOutMessageStream().popFrontMessage(type)) {
        printMessage(type, msg, inter);
        delete msg;
    }
}

bool runCode(aFuncore::Inter &inter, aFuncode::Code &code) {
    auto activation = new aFunrt::TopActivation(code, inter);
    inter.pushActivation(activation);
    return inter.runCode();
}

bool runCode(aFuncore::Inter &inter, aFuncore::Object *func) {
    auto func_obj = dynamic_cast<aFunrt::Function *>(func);
    auto activation = new aFunrt::FuncActivation(func_obj, inter);
    inter.pushActivation(activation);
    return inter.runCode();
}

void thread_test(aFuncore::Inter &son) {
    auto code = aFuncode::Code("run-code.aun");
    code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_p,
                                                             new aFuncode::Code::ByteCode(code, "test-var", 1), 0));
    runCode(son, code);
    progressInterEvent(son);
    aFuntool::fputs_stdout("\n");
}

class Func1 : public aFunrt::Function {
    class CallFunc1 : public CallFunction {
        aFuncode::Code &func_code;
        const aFuncode::Code::ByteCode *call_code;
        aFuncore::Inter &inter;
        std::list<ArgCodeList> *acl;
    public:
        CallFunc1(aFuncode::Code &func_code_, const aFuncode::Code::ByteCode *code_, aFuncore::Inter &inter_) : func_code{func_code_}, call_code{code_}, inter{inter_} {
            (void)call_code;  // 放置 call_code unused
            acl = new std::list<ArgCodeList>;
            if (code_ != nullptr) {
                ArgCodeList agr1 {code_->getSon()->toNext()};
                acl->push_front(agr1);
            }
        }

        std::list<ArgCodeList> *getArgCodeList(aFuncore::Inter &, aFuncore::Activation &, const aFuncode::Code::ByteCode *) override {
            return acl;
        }

        void runFunction() override {
            if (acl->empty())
                aFuntool::printf_stdout(0, "runFunction No AegCodeList\n");
            else
                aFuntool::printf_stdout(0, "runFunction : %p\n", acl->begin()->getObject());
            inter.pushActivation(new aFunrt::ExeActivation(func_code, inter));
        }

        ~CallFunc1() override {
            delete acl;
        }
    };

    aFuncode::Code func_code;
public:
    explicit Func1(aFuncore::Inter &inter_) : Object("Function", inter_), func_code {"run-code.aun"} {
        func_code.getByteCode()->connect(
                new aFuncode::Code::ByteCode(func_code, aFuncode::Code::ByteCode::block_p,
                                             new aFuncode::Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~Func1() override = default;

    CallFunc1 *getCallFunction(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter) override {
        return new CallFunc1(func_code, code, inter);
    }

    bool isInfix() override {return true;}

    void destruct(aFuncore::Inter &gc_inter) override {
        aFuntool::printf_stdout(0, "%p destruct\n", this);
        auto code = aFuncode::Code("run-code.aun");
        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_p,
                                                                 new aFuncode::Code::ByteCode(code, "test-var", 1), 0));
        runCode(gc_inter, code);
        progressInterEvent(gc_inter);
    };
};

class Literaler1 : public aFunrt::Literaler {
    aFuncode::Code func_code;
public:
    explicit Literaler1(aFuncore::Inter &inter_) : Object("Data", inter_), func_code{"run-code.aun"} {
        func_code.getByteCode()->connect(
                new aFuncode::Code::ByteCode(func_code, aFuncode::Code::ByteCode::block_p,
                                             new aFuncode::Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~Literaler1() override = default;

    void getObject(const std::string &literal, char prefix, aFuncore::Inter &inter, aFuncore::Activation &) override {
        aFuntool::cout << "Literaler1: " << literal  << "prefix: " << (prefix == aFuntool::NUL ? '-' : prefix) << "\n";
        inter.pushActivation(new aFunrt::ExeActivation(func_code, inter));
    }
};

class CBV1 : public aFunrt::CallBackVar {
    aFuncode::Code func_code;
public:
    explicit CBV1(aFuncore::Inter &inter_) : Object("CBV1", inter_), func_code{"run-code.aun"} {
        func_code.getByteCode()->connect(
                new aFuncode::Code::ByteCode(func_code, aFuncode::Code::ByteCode::block_p,
                                             new aFuncode::Code::ByteCode(func_code, "test-var", 1), 0));
    }

    ~CBV1() override = default;

    void callBack(aFuncore::Inter &inter, aFuncore::Activation &) override {
        aFuntool::cout << "CallBackVar callback\n";
        inter.pushActivation(new aFunrt::ExeActivation(func_code, inter));
    }
};

class ImportFunction : public aFunrt::Function {
    class CallFunc : public CallFunction {
        const aFuncode::Code::ByteCode *call_code;
        aFuncore::Inter &inter;
        std::list<ArgCodeList> *acl;
        std::string import;
    public:
        CallFunc(const aFuncode:: Code::ByteCode *code_, aFuncore::Inter &inter_) : call_code{code_}, inter{inter_} {
            if (code_ == nullptr ||
                code_->getSon() == nullptr ||
                code_->getSon()->toNext() == nullptr ||
                code_->getSon()->toNext()->getType() != aFuncode::Code::ByteCode::code_element)
                throw aFunrt::ArgumentError();
            acl = new std::list<ArgCodeList>;
            import = code_->getSon()->toNext()->getElement();
        }

        std::list<ArgCodeList> *getArgCodeList(aFuncore::Inter &,
                                               aFuncore::Activation &,
                                               const aFuncode::Code::ByteCode *) override {
            return acl;
        }

        void runFunction() override {
            auto &stream = inter.getActivation()->getDownStream();
            auto none = new Object("None", inter);
            stream.pushMessage("NORMAL", new aFunrt::NormalMessage(none));
            none->delReference();
            aFuntool::cout << "Import " << import << " : " << call_code << "\n";
        }

        ~CallFunc() override {
            delete acl;
        }
    };

public:
    AFUN_INLINE explicit ImportFunction(aFuncore::Inter &inter_) : Object("Function", inter_) {

    }

    AFUN_INLINE explicit ImportFunction(aFuncore::Environment &env_) : Object("Function", env_) {

    }

    ~ImportFunction() override = default;

    CallFunc *getCallFunction(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter) override {
        return new CallFunc(code, inter);
    }
};

int Main() {
    aFuncore::Environment env{};
    aFuncore::Inter inter {env};

    auto obj = new aFuncore::Object("Object", inter);
    inter.getEnvVarSpace().setObject("test-var", obj);
    aFuntool::cout << "obj: " << obj << "\n";
    obj->delReference();

    auto func = new Func1(inter);
    inter.getEnvVarSpace().setObject("test-func", func);
    aFuntool::cout << "func: " << func << "\n";
    func->delReference();

    auto literaler = new Literaler1(inter);
    inter.getEnvVarSpace().setObject("test-literaler", literaler);
    aFuntool::cout << "literaler: " << literaler << "\n";
    literaler->delReference();

    auto cbv = new CBV1(inter);
    inter.getEnvVarSpace().setObject("test-cbv", cbv);
    aFuntool::cout << "cbv: " << cbv << "\n";
    cbv->delReference();

    auto import = new ImportFunction(inter);
    inter.getEnvVarSpace().setObject("import", import);
    aFuntool::cout << "import: " << import << "\n";
    import->delReference();

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
        aFuntool::fputs_stdout("Test-1: block-p & get test-var\n");
        auto code = aFuncode::Code("run-code.aun");
        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_p,
                                                                 new aFuncode::Code::ByteCode(code, "test-var", 1), 0));
        runCode(inter, code);
        progressInterEvent(inter);
        aFuntool::fputs_stdout("\n");
    }

    {
        aFuntool::fputs_stdout("Test-2: block-c & run {test-func test-var}\n");
        auto code = aFuncode::Code("run-code.aun");

        auto arg = new aFuncode::Code::ByteCode(code, "test-func", 1);
        arg->connect(new aFuncode::Code::ByteCode(code, "test-var", 1));

        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_c, arg, 0));

        runCode(inter, code);
        progressInterEvent(inter);
        aFuntool::fputs_stdout("\n");
    }

    {
        aFuntool::fputs_stdout("Test-3: block-b & run [test-var test-func]\n");
        auto code = aFuncode::Code("run-code.aun");

        auto arg = new aFuncode::Code::ByteCode(code, "test-var", 1);
        arg->connect(new aFuncode::Code::ByteCode(code, "test-func", 1));

        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_b, arg, 0));
        runCode(inter, code);
        progressInterEvent(inter);
        aFuntool::fputs_stdout("\n");
    }

    {
        aFuntool::fputs_stdout("Test-4: test-literaler\n");
        inter.pushLiteral("data[0-9]", "test-literaler", false);
        auto code = aFuncode::Code("run-code.aun");
        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, "data4", 1));
        runCode(inter, code);
        progressInterEvent(inter);
        aFuntool::fputs_stdout("\n");
    }

    {
        aFuntool::fputs_stdout("Test-5: test-cbv\n");
        auto code = aFuncode::Code("run-code.aun");
        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, "test-cbv", 1));
        runCode(inter, code);
        progressInterEvent(inter);
        aFuntool::fputs_stdout("\n");
    }

    {
        aFuntool::fputs_stdout("Test-6: run-function\n");
        runCode(inter, func);
        progressInterEvent(inter);
        aFuntool::fputs_stdout("\n");
    }

    {
        aFuntool::fputs_stdout("Test-7: {import test}\n");
        auto code = aFuncode::Code("run-code.aun");

        auto arg = new aFuncode::Code::ByteCode(code, "import", 1);
        arg->connect(new aFuncode::Code::ByteCode(code, "test", 1));

        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_c, arg, 0));

        runCode(inter, code);
        progressInterEvent(inter);
        aFuntool::fputs_stdout("\n");
    }

    {
        /* 多线程 */
        aFuntool::fputs_stdout("Test-8: thread\n");
        auto son = aFuncore::Inter(inter);
        std::thread thread{thread_test, std::ref(son)};

        {
            auto code = aFuncode::Code("run-code.aun");
            code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_p,
                                                                     new aFuncode::Code::ByteCode(code, "test-var", 1), 0));
            runCode(inter, code);
            progressInterEvent(inter);
            aFuntool::fputs_stdout("\n");
        }

        thread.join();
    }

    /* 执行错误的代码 */
    {
        aFuntool::fputs_stdout("Test-a: error not var\n");
        auto code = aFuncode::Code("run-code.aun");
        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, "test-not-var", 1));
        runCode(inter, code);
        progressInterEvent(inter);
        aFuntool::fputs_stdout("\n");
    }

    inter.setInterExit();

    {
        auto code = aFuncode::Code("run-code.aun");
        code.getByteCode()->connect(new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_p,
                                                                 new aFuncode::Code::ByteCode(code, "test-var", 1), 0));
        runCode(inter, code);
        progressInterEvent(inter);
    }

    return 0;
}

int main() {
    int exit_code = 0;
    try {
        if (!aFunit::aFunInit()) {
            aFuntool::printf_stderr(0, "aFunlang init error.");
            aFuntool::aFunExit(EXIT_FAILURE);
        }

        auto factor = aFuntool::LogFactory(std::string(".") + aFuntool::SEP + "aFunlog", true);
        auto it_logger = aFuntool::Logger(factor, "aFun");
        auto core_logger = aFuntool::Logger(factor, "aFun-core");
        auto sys_logger = aFuntool::Logger(factor, "aFun-sys");
        aFunit::setAFunLogger(&it_logger);
        aFunrt::setRuntimeLogger(&core_logger);
        aFunparser::setParserLogger(&core_logger);
        aFuncore::setCoreLogger(&core_logger);
        aFuncode::setCodeLogger(&core_logger);
        aFuntool::setSysLogger(&sys_logger);

        exit_code = Main();
        aFuntool::aFunExit(exit_code);
    } catch (aFuntool::Exit &e) {
        exit_code = e.getExitCode();
    }
    aFuntool::aFunExitReal(exit_code);
}