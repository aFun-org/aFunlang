#include "env-var.hpp"
using namespace aFuncore;
using namespace aFuntool;

/**
 * 创建环境变量
 */
aFuncore::EnvVarSpace::EnvVarSpace() : count {0} {  // NOLINT lock 通过 pthread_rwlock_init 初始化
    pthread_rwlock_init(&lock, nullptr);
}

/**
 * 释放全部环境变量
 */
aFuncore::EnvVarSpace::~EnvVarSpace() {
    for (auto &i : var) {
        for (EnvVar *tmp = i, *next; tmp != nullptr; tmp = next) {
            next = tmp->next;
            delete tmp;
        }
    }
    pthread_rwlock_destroy(&lock);
}

/**
 * 获取环境变量文本
 * @param name 变量名
 * @param str 文本
 * @return 是否成功
 */
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

/**
 * 获取环境变量数值
 * @param name 变量名
 * @param num 文本
 * @return 是否成功
 */
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

/**
 * 设置环境变量文本
 * @param name 变量名
 * @param str 文本
 */
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

/**
 * 设置环境变量数值
 * @param name 变量名
 * @param num 数值
 */
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

