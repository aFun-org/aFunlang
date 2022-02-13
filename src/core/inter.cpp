#include "inter.h"
#include "core-activation.h"
#include "core-logger.h"
#include "core-exception.h"

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
}