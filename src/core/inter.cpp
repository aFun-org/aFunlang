#include "inter.h"
#include "core-activation.h"
#include "init.h"
#include "msg.h"
#include "core-exception.h"

namespace aFuncore {
    Inter::Inter(Environment &env_)
            : out{}, in{}, env{env_}{
        status = inter_creat;

        activation = nullptr;

        status = inter_init;
        env++;
    }

    Inter::Inter(const Inter &base_inter)
            : out{}, in{}, env{base_inter.env}{
        status = inter_creat;

        activation = nullptr;

        for (auto &i: base_inter.literal)
            literal.push_back(i);

        status = inter_normal;
        env++;
    }

    Inter::~Inter(){
        env--;
    }

    /**
     * 使能 (激活解释器)
     */
    void Inter::enable(){
        if (status == inter_init) {
            env.protect->setProtect(true);
            status = inter_normal;
        }
    }

    /**
     * 运行代码（直接运行activation）
     * @return
     */
    bool Inter::runCode(){
        if (status == inter_stop)
            status = inter_normal;

        while (activation != nullptr) {
            if (isInterStop()) {
                while (activation != nullptr) {
                    Activation *prev = activation->toPrev();
                    delete activation;
                    activation = prev;
                }
                return false;
            }

            const Code::ByteCode *code = nullptr;
            Activation::ActivationStatus as = activation->getCode(code);
            switch (as) {
                case Activation::as_end: {
                    Activation *prev = activation->toPrev();
                    delete activation;
                    activation = prev;
                    break;
                }
                case Activation::as_run:
                    activation->runCode(code);
                    break;
                case Activation::as_end_run:
                    activation->endRun();
                    break;
                default:
                    errorLog(aFunCoreLogger, "Error activation status.");
                    activation->getDownStream().pushMessage("ERROR",
                                                            new ErrorMessage("RuntimeError", "Error activation status.", activation));
                    break;
            }
        }
        return true;
    }

    /**
     * 运行代码
     * @param code 代码
     * @return
     */
    bool Inter::runCode(const Code &code){
        if (activation != nullptr) {
            errorLog(aFunCoreLogger, "Run code with activation");
            return false;
        }

        new TopActivation(code, *this);
        return runCode();
    }

    /**
     * 函数调用
     * @param code 代码
     * @return
     */
    bool Inter::runCode(Object *func){
        if (activation != nullptr) {
            errorLog(aFunCoreLogger, "Run function with activation");
            return false;
        }

        Function *func_obj = dynamic_cast<Function *>(func);
        if (func_obj == nullptr) {
            errorLog(aFunCoreLogger, "Run without function");
            return false;
        }

        new FuncActivation(func_obj, *this);
        return runCode();
    }

    /**
     * 检查字面量是否匹配
     * @param element 字面量
     * @return
     */
    bool Inter::checkLiteral(const std::string &element) const{
        if (literal.empty())
            return false;

        for (auto &i: literal) {
            try {
                if (i.rg.match(element) != 1)
                    continue;
                return true;
            } catch (aFuntool::RegexException &e) {
                continue;
            }
        }
        return false;
    }

    /**
     * 检查字面量正则匹配
     * @param element 字面量
     * @param literaler 函数
     * @param in_protect 是否保护空间
     * @return
     */
    bool Inter::checkLiteral(const std::string &element, std::string &literaler, bool &in_protect) const{
        if (literal.empty())
            return false;

        for (auto &i: literal) {
            try {
                if (i.rg.match(element) != 1)
                    continue;
                literaler = i.literaler;
                in_protect = i.in_protect;
                return true;
            } catch (aFuntool::RegexException &e) {
                continue;
            }
        }
        return false;
    }

    bool Inter::pushLiteral(const std::string &pattern, const std::string &literaler, bool in_protect){
        try {
            literal.push_front({aFuntool::Regex(pattern), pattern, literaler, in_protect});
        } catch (aFuntool::RegexException &e) {
            return false;
        }
        return true;
    }

    Environment::Environment(int argc, char **argv)
        : obj{nullptr}, var{nullptr}, varspace{nullptr},
          protect{new ProtectVarSpace(*this)}, global{new VarSpace(*this)},
          global_varlist{new VarList(protect)}, destruct{false} {
        global_varlist->push(global);
        reference = 0;

        envvar.setNumber("sys:gc-runtime", 2);
        envvar.setString("sys:prefix", "''");  // 引用，顺序执行
        envvar.setNumber("sys:exit-code", 0);
        envvar.setNumber("sys:argc", argc);
        envvar.setNumber("sys:error_std", 0);

        for (int i = 0; i < argc; i++) {
            char buf[20];
            snprintf(buf, 10, "sys:arg%d", i);
            envvar.setString(buf, argv[i]);
        }
    }

    Environment::~Environment() noexcept(false) {
        if (reference != 0)
            throw EnvironmentDestructException();

        if (destruct)
            return;

        delete global_varlist;

        Object::destruct(obj);
        Var::destruct(var);
        VarSpace::destruct(varspace);

        obj = nullptr;
        var = nullptr;
        varspace = nullptr;
        destruct = true;
    }
}