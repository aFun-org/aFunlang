#include "var.hpp"
#include "inter.hpp"
#include "__gc.hpp"

using namespace aFuncore;
using namespace aFuntool;


aFuncore::Var::Var(Object *data_, Inter *inter_) : data{data_}, inter{inter_->base} {
    addObject(inter->getGcRecord()->var);
}

aFuncore::VarSpace::VarSpace(Inter *inter_) : count{0}, var{}, inter{inter_->base} {
    addObject(inter->getGcRecord()->varspace);
}

Var *aFuncore::VarSpace::findVar(const std::string &name){
    size_t index = time33(name) % VAR_HASH_SIZE;
    for (auto tmp = var[index]; tmp != nullptr; tmp = tmp->next) {
        if (tmp->name == name)
            return tmp->var;
    }
    return nullptr;
}

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

VarOperationFlat aFuncore::ProtectVarSpace::defineVar(const std::string &name, Object *data){
    if (is_protect)
        return findVar(name) ? vof_redefine_var : vof_fail;
    return VarSpace::defineVar(name, data);
}

VarOperationFlat aFuncore::ProtectVarSpace::defineVar(const std::string &name, Var *data){
    if (is_protect)
        return findVar(name) ? vof_redefine_var : vof_fail;
    return VarSpace::defineVar(name, data);
}

VarOperationFlat aFuncore::ProtectVarSpace::setVar(const std::string &name, Object *data){
    if (is_protect)
        return findVar(name) ? vof_fail : vof_not_var;
    return VarSpace::setVar(name, data);
}

VarOperationFlat aFuncore::ProtectVarSpace::delVar(const std::string &name){
    if (is_protect)
        return findVar(name) ? vof_fail : vof_not_var;
    return VarSpace::delVar(name);
}


Var *aFuncore::VarList::findVar(const std::string &name){
    Var *ret = nullptr;
    for (auto tmp = this; tmp != nullptr && ret == nullptr; tmp = tmp->next)
        ret = tmp->varspace->findVar(name);
    return ret;
}

bool aFuncore::VarList::defineVar(const std::string &name, Object *data){
    VarOperationFlat ret = vof_fail;
    for (auto tmp = this; tmp != nullptr && ret == vof_fail; tmp = tmp->next)
        ret = tmp->varspace->defineVar(name, data);
    return ret == vof_success;
}

bool aFuncore::VarList::defineVar(const std::string &name, Var *data){
    VarOperationFlat ret = vof_fail;
    for (auto tmp = this; tmp != nullptr && ret == vof_fail; tmp = tmp->next)
        ret = tmp->varspace->defineVar(name, data);
    return ret == vof_success;
}

bool aFuncore::VarList::setVar(const std::string &name, Object *data){
    VarOperationFlat ret = vof_not_var;
    for (auto tmp = this; tmp != nullptr && ret == vof_not_var; tmp = tmp->next)
        ret = tmp->varspace->setVar(name, data);
    return ret == vof_success;
}

bool aFuncore::VarList::delVar(const std::string &name){
    VarOperationFlat ret = vof_not_var;
    for (auto tmp = this; tmp != nullptr && ret == vof_not_var; tmp = tmp->next)
        ret = tmp->varspace->delVar(name);
    return ret == vof_success;
}

void aFuncore::VarList::disconnect(VarList *varlist){
    for (VarList *tmp = this; tmp != nullptr; tmp = tmp->next) {
        if (tmp->next == varlist) {
            tmp->next = nullptr;
            return;
        }
    }
}

void VarList::destructAll(){
    for (VarList *tmp=this, *n; tmp != nullptr; tmp = n) {
        n = tmp->next;
        delete tmp;
    }
}
