#include "inter.hpp"
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
    gc_runtime = envvar->findVar("sys:gc-runtime");
    gc_runtime->num = 2;
    prefix = envvar->findVar("sys:prefix");
    exit_code = envvar->findVar("sys:exit-code");
    exit_code->num = 0;
    this->argc = envvar->findVar("sys:gc-runtime");
    this->argc->num = argc;
    error_std = envvar->findVar("sys:gc-runtime");
    error_std->num = 0;

    for (int i = 0; i < argc; i++) {
        char buf[20];
        snprintf(buf, 10, "sys:arg%d", i);
        auto tmp = envvar->findVar(buf);
        tmp->str = argv[i];
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

void Inter::enable(){
    if (status == inter_init) {
        protect->setProtect(true);
        status = inter_normal;
    }
}

bool Inter::runCode(){
    while (activation != nullptr) {
        if (isExit()) {
            // TODO-szh 弹出所有activation
            return false;
        }

        Code *code = activation->getCode();
        if (code == nullptr) {  // activation 执行完成
            Activation *prev = activation->toPrev();
            delete activation;
            activation = prev;
            continue;
        }

        auto code_type = code->getType();
        if (code_type == code_start)
            continue;

        Message *msg = activation->getDownStream()->popMessage("NORMAL");
        if (msg == nullptr) {
            // ... 出现异常
        }

        delete msg;

        if (code_type == code_element) {
            std::string func;
            bool in_protect = false;
            if (checkLiteral(code->getElement(), func, in_protect)) {
                // ...
            } else {
                auto varlist = activation->getVarlist();
                Object *obj = nullptr;
                if (varlist != nullptr)
                    obj = varlist->findObject(code->getElement());
                if (obj != nullptr)
                    activation->getDownStream()->pushMessage(new NormalMessage(obj));
            }

        } else switch (code->getBlockType()) {
            case block_p:
                break;
            case block_b:
                break;
            case block_c:
                break;
            default:
                errorLog(aFunCoreLogger, "Error block type.");
                break;
        }
    }
    return true;
}

bool Inter::runCode(Code *code){
    if (activation != nullptr) {
        errorLog(aFunCoreLogger, "Run code with activation");
        return false;
    }

    new TopActivation(code, this);
    return runCode();
}

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

