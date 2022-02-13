#include "inter.h"
#include "core-activation.h"
#include "core-logger.h"
#include "core-message-stream.h"
#include "core-exception.h"
#include "object.h"

namespace aFuncore {
    Inter::Inter(Environment &env_)
            : status{inter_init}, env{env_}, activation{nullptr}, out{}, in{} {
        env++;
    }

    Inter::Inter(const Inter &base_inter)
            : status{inter_init}, env{base_inter.env}, activation{nullptr}, out{}, in{}{
        for (auto &i: base_inter.literal)
            literal.push_back(i);
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
                while (activation != nullptr)
                    delete popActivation();
                return false;
            }

            const aFuncode::Code::ByteCode *code = nullptr;
            Activation::ActivationStatus as = activation->getCode(code);
            switch (as) {
                case Activation::as_end: {
                    delete popActivation();
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
                    delete popActivation();
                    break;
            }
        }
        return true;
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
            } catch (aFuntool::RegexException &) {
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
            } catch (aFuntool::RegexException &) {
                continue;
            }
        }
        return false;
    }

    bool Inter::pushLiteral(const std::string &pattern, const std::string &literaler, bool in_protect){
        try {
            literal.push_front({aFuntool::Regex(pattern), pattern, literaler, in_protect});
        } catch (aFuntool::RegexException &) {
            return false;
        }
        return true;
    }

    void Environment::gcThread() {
        while(true) {
            std::queue<Object *> del;
            std::queue<Object *> des;
            {
                std::unique_lock<std::mutex> mutex{lock};
                if (destruct)
                    break;
                Object::checkReachable(gc);
                Object::setReachable(gc, des, del);
            }
            Object::deleteUnreachable(del);
            Object::destructUnreachable(des, gc_inter);

            int32_t intervals = 1000;
            env_var.findNumber("sys:gc-intervals", intervals);
            if (intervals < 100)
                intervals = 100;
            std::this_thread::sleep_for(std::chrono::milliseconds(intervals));
        }

        Object::destructAll(gc, gc_inter); /* 不需要mutex锁 */
    }

    Environment::Environment(int argc, char **argv)
        : reference{0}, destruct{false}, gc_inter{*(new Inter(*this))}, env_var{*new EnvVarSpace()} {
        /* 生成 gc_inter 后, reference == 1 */

        env_var.setNumber("sys:gc-intervals", 1000);
        env_var.setNumber("sys:exit-code", 0);
        env_var.setNumber("sys:argc", argc);
        for (int i = 0; i < argc; i++) {
            char buf[20];
            snprintf(buf, 10, "sys:arg%d", i);
            env_var.setString(buf, argv[i]);
        }

        gc_thread = std::thread([this](){this->gcThread();});
    }

    Environment::~Environment() noexcept(false) {
        {   /* 使用互斥锁, 防止与gc线程出现不同步的情况 */
            std::unique_lock<std::mutex> mutex{lock};
            if (reference != 1)  // gc_inter 有一个引用
                throw EnvironmentDestructException();

            if (destruct)
                return;

            destruct = true;
        }

        gc_thread.join();
        delete &gc_inter;
        delete &env_var;

        Object::deleteAll(gc); /* 不需要mutex锁 */

        if (reference != 0)
            throw EnvironmentDestructException();
    }
}