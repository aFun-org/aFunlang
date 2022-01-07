#include "var.h"
#include "inter.h"

using namespace aFuncore;
using namespace aFuntool;


aFuncore::Var::Var(Object *data_, Inter *inter_) : data{data_}, inter{inter_->base} {
    addObject(inter->getGcRecord()->var);
}

aFuncore::VarSpace::VarSpace(Inter *inter_) : count{0}, var{}, inter{inter_->base} {
    addObject(inter->getGcRecord()->varspace);
}

/**
 * 访问指定变量
 * @param name 变量名
 * @return
 */
Var *aFuncore::VarSpace::findVar(const std::string &name){
    size_t index = time33(name) % VAR_HASH_SIZE;
    for (auto tmp = var[index]; tmp != nullptr; tmp = tmp->next) {
        if (tmp->name == name)
            return tmp->var;
    }
    return nullptr;
}

/**
 * 定义变量
 * @param name 变量名
 * @param data 变量（Object）
 * @return
 */
VarOperationFlat aFuncore::VarSpace::defineVar(const std::string &name, Object *data){
    size_t index = time33(name) % VAR_HASH_SIZE;
    auto tmp = &var[index];
    for (NULL; *tmp != nullptr; tmp = &(*tmp)->next) {
        if ((*tmp)->name == name)
            return vof_redefine_var;
    }
    (*tmp) = new VarCup;
    (*tmp)->name = name;
    (*tmp)->var = new Var(data, inter);
    count++;
    return vof_success;
}

/**
 * 定义变量
 * @param name 变量名
 * @param data 变量（Var）
 * @return
 */
VarOperationFlat aFuncore::VarSpace::defineVar(const std::string &name, Var *data){
    size_t index = time33(name) % VAR_HASH_SIZE;
    auto tmp = &var[index];
    for (NULL; *tmp != nullptr; tmp = &(*tmp)->next) {
        if ((*tmp)->name == name)
            return vof_redefine_var;
    }
    (*tmp) = new VarCup;
    (*tmp)->name = name;
    (*tmp)->var = data;
    count++;
    return vof_success;
}

/**
 * 设定变量的值
 * @param name 变量名
 * @param data 变量
 * @return
 */
VarOperationFlat aFuncore::VarSpace::setVar(const std::string &name, Object *data){
    size_t index = time33(name) % VAR_HASH_SIZE;
    for (auto tmp = var[index]; tmp != nullptr; tmp = tmp->next) {
        if (tmp->name == name) {
            tmp->var->setData(data);
            return vof_success;
        }
    }
    return vof_not_var;
}

/**
 * 删除变量
 * @param name 变量名
 * @return
 */
VarOperationFlat aFuncore::VarSpace::delVar(const std::string &name){
    size_t index = time33(name) % VAR_HASH_SIZE;
    for (auto tmp = var[index]; tmp != nullptr && tmp->next != nullptr; tmp = tmp->next) {
        if (tmp->next->name == name) {
            auto del = tmp->next;
            tmp->next = del->next;
            delete del;  // 删除 VarCup
            count--;
            return vof_success;
        }
    }
    return vof_not_var;
}

aFuncore::VarSpace::~VarSpace(){
    for (auto &cup : var) {
        for (VarCup *next; cup != nullptr; cup = next) {
            next = cup->next;
            delete cup;
        }
    }
}

aFuncore::VarList::VarList(VarList *varlist) {
    for (auto &t : varlist->varspace)
        this->varspace.push_back(t);
}

/**
 * 定义变量
 * 若启用保护且变量名存在，则返回错误redefine
 * 若启用保护则返回错误fail
 * @param name 变量名
 * @param data 变量（Object）
 * @return
 */
VarOperationFlat aFuncore::ProtectVarSpace::defineVar(const std::string &name, Object *data){
    if (is_protect)
        return findVar(name) ? vof_redefine_var : vof_fail;
    return VarSpace::defineVar(name, data);
}

/**
 * 定义变量
 * 若启用保护且变量名存在，则返回错误redefine
 * 若启用保护则返回错误fail
 * @param name 变量名
 * @param data 变量（Var）
 * @return
 */
VarOperationFlat aFuncore::ProtectVarSpace::defineVar(const std::string &name, Var *data){
    if (is_protect)
        return findVar(name) ? vof_redefine_var : vof_fail;
    return VarSpace::defineVar(name, data);
}

/**
 * 设定变量的值
 * 若启用保护且变量名存在，则返回错误fail
 * 若启用保护则返回错误 not_var
 * @param name 变量名
 * @param data 变量（Var）
 * @return
 */
VarOperationFlat aFuncore::ProtectVarSpace::setVar(const std::string &name, Object *data){
    if (is_protect)
        return findVar(name) ? vof_fail : vof_not_var;
    return VarSpace::setVar(name, data);
}

/**
 * 删除变量
 * 若启用保护且变量名存在，则返回错误fail
 * 若启用保护则返回错误 not_var
 * @param name 变量名
 * @param data 变量（Var）
 * @return
 */
VarOperationFlat aFuncore::ProtectVarSpace::delVar(const std::string &name){
    if (is_protect)
        return findVar(name) ? vof_fail : vof_not_var;
    return VarSpace::delVar(name);
}

/**
 * 访问变量
 * @param name 变量名
 * @return
 */
Var *aFuncore::VarList::findVar(const std::string &name){
    Var *ret = nullptr;
    for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == nullptr; tmp++)
        ret = (*tmp)->findVar(name);
    return ret;
}

/**
 * 定义变量
 * 若定义出现redefine则退出报错
 * 若出现fail则跳到下一个变量空间尝试定义
 * @param name 变量名
 * @param data 变量（Object）
 * @return
 */
bool aFuncore::VarList::defineVar(const std::string &name, Object *data){
    VarOperationFlat ret = vof_fail;
    for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == vof_fail; tmp++)
        ret = (*tmp)->defineVar(name, data);
    return ret == vof_success;
}

/**
 * 定义变量
 * 若定义出现redefine则退出报错
 * 若出现fail则跳到下一个变量空间尝试定义
 * @param name 变量名
 * @param data 变量（Var）
 * @return
 */
bool aFuncore::VarList::defineVar(const std::string &name, Var *data){
    VarOperationFlat ret = vof_fail;
    for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == vof_fail; tmp++)
        ret = (*tmp)->defineVar(name, data);
    return ret == vof_success;
}

/**
 * 设置变量的值
 * 若not_var则跳到下一个变量空间
 * 若fail则结束
 * @param name 变量名
 * @param data 数据
 * @return
 */
bool aFuncore::VarList::setVar(const std::string &name, Object *data){
    VarOperationFlat ret = vof_not_var;
    for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == vof_not_var; tmp++)
        ret = (*tmp)->setVar(name, data);
    return ret == vof_success;
}

/**
 * 删除变量
 * 若not_var则跳到下一个变量空间
 * 若fail则结束
 * @param name
 * @return
 */
bool aFuncore::VarList::delVar(const std::string &name){
    VarOperationFlat ret = vof_not_var;
    for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == vof_not_var; tmp++)
        ret = (*tmp)->delVar(name);
    return ret == vof_success;
}

void VarList::connect(VarList *varlist){
    for (auto &t : varlist->varspace)
        this->varspace.push_back(t);
}
