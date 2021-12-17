#include "inter.hpp"
#include "__gc.hpp"

using namespace aFuncore;
using namespace aFuntool;


Inter::Inter(int argc, char **argv, ExitMode em) : status_lock{}, monitor_lock{}, monitor_cond{} {
    status = inter_creat;
    pthread_mutex_init(&status_lock, nullptr);

    gc = new GcRecord;
    gc->obj = nullptr;
    gc->var = nullptr;
    gc->varspace = nullptr;

    activation = nullptr;

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

    is_derive = false;
    base = this;
    result = nullptr;
    son_inter = new std::list<Inter *>;

    monitor = 0;

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

Var *Inter::findGlobalVar(const std::string &name) {
    return global->findVar(name);
}

VarOperationFlat Inter::defineGlobalVar(const std::string &name, Object *data) {
    return global->defineVar(name, data);
}

VarOperationFlat Inter::defineGlobalVar(const std::string &name, Var *data) {
    return global->defineVar(name, data);
}

VarOperationFlat Inter::setGlobalVar(const std::string &name, Object *data) {
    return global->setVar(name, data);
}

VarOperationFlat Inter::delGlobalVar(const std::string &name) {
    return global->delVar(name);
}

Object *Inter::findGlobalObject(const std::string &name) {
    return global->findObject(name);
}

bool Inter::runCode(Code *code){
    return 0;
}
