#include "inter.h"
#include "activation.h"
#include "init.h"
#include "env-var.h"
#include "var.h"
#include "msg.h"

using namespace aFuncore;
using namespace aFuntool;

Inter::Inter(int argc, char **argv, ExitMode em)
    : base{*this}, is_derive{false}, out{}, in{}, envvar{*(new EnvVarSpace())} {
    status = inter_creat;

    gc = new GcRecord;
    gc->obj = nullptr;
    gc->var = nullptr;
    gc->varspace = nullptr;

    activation = nullptr;

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

    result = nullptr;
    son_inter = new std::list<Inter *>;

    exit_flat = ef_none;
    exit_mode = em;

    protect = new ProtectVarSpace(*this);  // 放到最后
    global = new VarSpace(*this);  // 放到最后
    global_varlist = new VarList(protect);
    global_varlist->push(global);

    status = inter_init;
}

Inter::Inter(const Inter &base_inter, ExitMode em)
        : base{base_inter.base}, is_derive{true}, out{}, in{}, envvar{base_inter.base.envvar} {
    status = inter_creat;

    gc = base.gc;

    activation = nullptr;

    for(auto &i : base.literal)
        literal.push_back(i);

    result = nullptr;
    son_inter = nullptr;
    base.son_inter->push_back(this);

    exit_flat = ef_none;
    exit_mode = em;

    protect = base.protect;  // 放到最后
    global = base.global;  // 放到最后
    global_varlist = base.global_varlist;

    status = inter_init;
}

Inter::~Inter(){
    if (!is_derive) {
        delete global_varlist;

        Object::destruct(gc->obj);
        Var::destruct(gc->var);
        VarSpace::destruct(gc->varspace);

        delete gc;
        delete son_inter;
        delete &envvar;
    }
}

/**
 * 使能 (激活解释器)
 */
void Inter::enable(){
    if (status == inter_init) {
        protect->setProtect(true);
        status = inter_normal;
    }
}

/**
 * 运行代码（直接运行activation）
 * @return
 */
bool Inter::runCode(){
    while (activation != nullptr) {
        Code *code = nullptr;
        ActivationStatus as = activation->getCode(code);
        switch (as) {
            case as_end: {
                Activation *prev = activation->toPrev();
                delete activation;
                activation = prev;
                break;
            }
            case as_run:
                activation->runCode(code);
                break;
            case as_end_run:
                activation->endRun();
                break;
            default:
                errorLog(aFunCoreLogger, "Error activation status.");
                activation->getDownStream().pushMessage(new ErrorMessage("RuntimeError", "Error activation status.", activation));
                break;
        }

        if (isExit()) {
            while (activation != nullptr) {
                Activation *prev = activation->toPrev();
                delete activation;
                activation = prev;
            }
            return false;
        }
    }
    return true;
}

/**
 * 运行代码
 * @param code 代码
 * @return
 */
bool Inter::runCode(Code *code){
    if (activation != nullptr) {
        errorLog(aFunCoreLogger, "Run code with activation");
        return false;
    }

    new TopActivation(code, *this);
    return runCode();
}

/**
 * 检查字面量是否匹配
 * @param element 字面量
 * @return
 */
bool Inter::checkLiteral(const std::string &element) const {
    if (literal.empty())
        return false;

    for(auto &i : literal){
        try {
            if (i.rg.match(element) != 1)
                continue;
            return true;
        } catch (RegexException &e) {
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
bool Inter::checkLiteral(const std::string &element, std::string &literaler, bool &in_protect) const {
    if (literal.empty())
        return false;

    for(auto &i : literal){
        try {
            if (i.rg.match(element) != 1)
                continue;
            literaler = i.literaler;
            in_protect = i.in_protect;
            return true;
        } catch (RegexException &e) {
            continue;
        }
    }
    return false;
}

bool Inter::pushLiteral(const std::string &pattern, const std::string &literaler, bool in_protect){
    try {
        literal.push_front({Regex(pattern), pattern, literaler, in_protect});
    } catch (RegexException &e) {
        return false;
    }
    return true;
}

