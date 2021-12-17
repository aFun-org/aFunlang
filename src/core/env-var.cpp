#include "env-var.hpp"
using namespace aFuncore;
using namespace aFuntool;

aFuncore::EnvVarSpace::EnvVarSpace() : count {0} {  // NOLINT lock 通过 pthread_rwlock_init 初始化
    pthread_rwlock_init(&lock, nullptr);
}

aFuncore::EnvVarSpace::~EnvVarSpace() {
    for (auto &i : var) {
        for (EnvVar *tmp = i, *next; tmp != nullptr; tmp = next) {
            next = tmp->next;
            delete tmp;
        }
    }
    pthread_rwlock_destroy(&lock);
}

bool EnvVarSpace::findString(const std::string &name, std::string &str) const{
    size_t index = time33(name) % ENV_VAR_HASH_SIZE;
    for (auto tmp = var[index]; tmp != nullptr; tmp = tmp->next) {
        if (name == tmp->name) {
            str = tmp->str;
            return true;
        }
    }
    return false;
}

bool EnvVarSpace::findNumber(const std::string &name, int32_t &num) const{
    size_t index = time33(name) % ENV_VAR_HASH_SIZE;
    for (auto tmp = var[index]; tmp != nullptr; tmp = tmp->next) {
        if (name == tmp->name) {
            num = tmp->num;
            return true;
        }
    }
    return false;
}

void EnvVarSpace::setString(const std::string &name, const std::string &str){
    size_t index = time33(name) % ENV_VAR_HASH_SIZE;
    auto tmp = &var[index];
    for (NULL; *tmp != nullptr; tmp = &((*tmp)->next)) {
        if (name == (*tmp)->name) {
            (*tmp)->str = str;
            return;
        }
    }

    (*tmp) = new EnvVar;
    (*tmp)->name = name;
    (*tmp)->str = str;
}

void EnvVarSpace::setNumber(const std::string &name, int32_t num){
    size_t index = time33(name) % ENV_VAR_HASH_SIZE;
    auto tmp = &var[index];
    for (NULL; *tmp != nullptr; tmp = &((*tmp)->next)) {
        if (name == (*tmp)->name) {
            (*tmp)->num = num;
            return;
        }
    }

    (*tmp) = new EnvVar;
    (*tmp)->name = name;
    (*tmp)->num = num;
}

EnvVarSpace::EnvVar *EnvVarSpace::findVar(const std::string &name){
    size_t index = time33(name) % ENV_VAR_HASH_SIZE;
    auto tmp = &var[index];
    for (NULL; *tmp != nullptr; tmp = &((*tmp)->next)) {
        if (name == (*tmp)->name) {
            return *tmp;
        }
    }

    (*tmp) = new EnvVar;
    (*tmp)->name = name;
    return (*tmp);
}


