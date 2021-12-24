﻿#include "inter.hpp"
#include "init.hpp"
#include "__gc.hpp"
using namespace aFuncore;
using namespace aFuntool;

Inter::Inter(int argc, char **argv, ExitMode em)
     : base{this}, is_derive{false}, status_lock{}, monitor{}, monitor_lock{}, monitor_cond{} {
    status = inter_creat;
    pthread_mutex_init(&status_lock, nullptr);

    gc = new GcRecord;
    gc->obj = nullptr;
    gc->var = nullptr;
    gc->varspace = nullptr;

    activation = nullptr;
    literal = new std::list<LiteralRegex>;

    envvar = new EnvVarSpace();
    envvar->setNumber("sys:gc-runtime", 2);
    envvar->setString("sys:prefix", "''");  // 引用，顺序执行
    envvar->setNumber("sys:exit-code", 0);
    envvar->setNumber("sys:argc", argc);
    envvar->setNumber("sys:error_std", 0);

    for (int i = 0; i < argc; i++) {
        char buf[20];
        snprintf(buf, 10, "sys:arg%d", i);
        envvar->setString(buf, argv[i]);
    }

    result = nullptr;
    son_inter = new std::list<Inter *>;

    exit_flat = ef_none;
    exit_mode = em;

    pthread_mutex_init(&monitor_lock, nullptr);
    pthread_cond_init(&monitor_cond, nullptr);

    protect = new ProtectVarSpace(this);  // 放到最后
    global = new VarSpace(this);  // 放到最后
    global_varlist = (new VarList(global))->connect(new VarList(protect));
    status = inter_init;
}

Inter::~Inter(){
    pthread_mutex_destroy(&status_lock);
    pthread_mutex_destroy(&monitor_lock);
    pthread_cond_destroy(&monitor_cond);

    if (!is_derive) {
        global_varlist->destructAll();

        Object::destruct(gc->obj);
        Var::destruct(gc->var);
        VarSpace::destruct(gc->varspace);

        delete literal;
        delete gc;
        delete son_inter;
        delete envvar;
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
        if (isExit()) {
            while (activation != nullptr) {
                Activation *prev = activation->toPrev();
                delete activation;
                activation = prev;
            }
            return false;
        }

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
            default:
                errorLog(aFunCoreLogger, "Error activation status.");
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
bool Inter::runCode(Code *code){
    if (activation != nullptr) {
        errorLog(aFunCoreLogger, "Run code with activation");
        return false;
    }

    new TopActivation(code, this);
    return runCode();
}

/**
 * 检查字面量正则匹配
 * @param element 字面量
 * @param func 函数
 * @param in_protect 是否保护空间
 * @return
 */
bool Inter::checkLiteral(const std::string &element, std::string &func, bool &in_protect) const {
    if (literal->empty())
        return false;

    auto it = literal->begin();
    auto end = literal->end();

    for(NULL;it != end;it++){
        try {
            if (it->rg->match(element) != 1)
                continue;
            func = it->func;
            in_protect = it->in_protect;
            return true;
        } catch (RegexException &e) {
            continue;
        }
    }
    return false;
}

