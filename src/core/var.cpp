#include "var.h"
#include "value.h"
#include "inter.h"
#include "init.h"

namespace aFuncore {
    Var::Var(Object *data_, Inter &inter) : data{data_}, env{inter.getEnvironment()}{
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.push_back(this);
    }
    
    Var::Var(Object *data_, Environment &env_) : data{data_}, env{env_}{
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.push_back(this);
    }

    Var::~Var() {
        if (getReference() != 0)
            warningLog(aFunCoreLogger, "Var %p destruct reference: %d", this, getReference());
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.remove(this);
    }
    
    VarSpace::VarSpace(Inter &inter) : env{inter.getEnvironment()}{
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.push_back(this);
    }
    
    VarSpace::VarSpace(Environment &env_) : env{env_}{
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.push_back(this);
    }

    VarSpace::~VarSpace() {
        if (getReference() != 0)
            warningLog(aFunCoreLogger, "VarSpace %p destruct reference: %d", this, getReference());
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.remove(this);
    }

    void Var::linkObject(std::queue<GcObjectBase *> &queue) {
        queue.push(getData());
    }

    /**
     * 访问指定变量
     * @param name 变量名
     * @return
     */
    Var *VarSpace::findVar(const std::string &name){
        std::unique_lock<std::mutex> mutex{lock};
        auto v = var.find(name);
        if (v == var.end())
            return nullptr;
        return v->second;
    }
    
    /**
     * 定义变量
     * @param name 变量名
     * @param data 变量（Object）
     * @return
     */
    VarSpace::VarOperationFlat VarSpace::defineVar(const std::string &name, Object *data) {
        std::unique_lock<std::mutex> mutex{lock};
        if (var.find(name) != var.end())
            return vof_redefine_var;
        auto new_var = new Var(data, env);
        var.emplace(name, new_var);
        new_var->delReference();
        return vof_success;
    }
    
    /**
     * 定义变量
     * @param name 变量名
     * @param data 变量（Var）
     * @return
     */
    VarSpace::VarOperationFlat VarSpace::defineVar(const std::string &name, Var *data){
        std::unique_lock<std::mutex> mutex{lock};
        if (var.find(name) != var.end())
            return vof_redefine_var;
        var.emplace(name, data);
        return vof_success;
    }
    
    /**
     * 设定变量的值
     * @param name 变量名
     * @param data 变量
     * @return
     */
    VarSpace::VarOperationFlat VarSpace::setVar(const std::string &name, Object *data){
        std::unique_lock<std::mutex> mutex{lock};
        auto v = var.find(name);
        if (v == var.end())
            return vof_not_var;
        v->second->setData(data);
        return vof_success;
    }
    
    /**
     * 删除变量
     * @param name 变量名
     * @return
     */
    VarSpace::VarOperationFlat VarSpace::delVar(const std::string &name){
        std::unique_lock<std::mutex> mutex{lock};
        auto v = var.find(name);
        if (v == var.end())
            return vof_not_var;
        var.erase(v);
        return vof_success;
    }

    void VarSpace::linkObject(std::queue<GcObjectBase *> &queue) {
        for (auto tmp : var)
            queue.push(tmp.second);
    }
    
    VarList::VarList(VarList *varlist){
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &t: varlist->varspace)
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
    VarSpace::VarOperationFlat ProtectVarSpace::defineVar(const std::string &name, Object *data){
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
    VarSpace::VarOperationFlat ProtectVarSpace::defineVar(const std::string &name, Var *data){
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
    VarSpace::VarOperationFlat ProtectVarSpace::setVar(const std::string &name, Object *data){
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
    VarSpace::VarOperationFlat ProtectVarSpace::delVar(const std::string &name){
        if (is_protect)
            return findVar(name) ? vof_fail : vof_not_var;
        return VarSpace::delVar(name);
    }
    
    /**
     * 访问变量
     * @param name 变量名
     * @return
     */
    Var *VarList::findVar(const std::string &name){
        std::unique_lock<std::mutex> mutex{lock};
        Var *ret = nullptr;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == nullptr; tmp++) {
            mutex.unlock();
            ret = (*tmp)->findVar(name);
            mutex.lock();
        }
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
    bool VarList::defineVar(const std::string &name, Object *data){
        std::unique_lock<std::mutex> mutex{lock};
        VarSpace::VarOperationFlat ret = VarSpace::vof_fail;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_fail; tmp++) {
            mutex.unlock();
            ret = (*tmp)->defineVar(name, data);
            mutex.lock();
        }
        return ret == VarSpace::vof_success;
    }
    
    /**
     * 定义变量
     * 若定义出现redefine则退出报错
     * 若出现fail则跳到下一个变量空间尝试定义
     * @param name 变量名
     * @param data 变量（Var）
     * @return
     */
    bool VarList::defineVar(const std::string &name, Var *data){
        std::unique_lock<std::mutex> mutex{lock};
        VarSpace::VarOperationFlat ret = VarSpace::vof_fail;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_fail; tmp++) {
            mutex.unlock();
            ret = (*tmp)->defineVar(name, data);
            mutex.lock();
        }
        return ret == VarSpace::vof_success;
    }
    
    /**
     * 设置变量的值
     * 若not_var则跳到下一个变量空间
     * 若fail则结束
     * @param name 变量名
     * @param data 数据
     * @return
     */
    bool VarList::setVar(const std::string &name, Object *data){
        std::unique_lock<std::mutex> mutex{lock};
        VarSpace::VarOperationFlat ret = VarSpace::vof_not_var;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_not_var; tmp++) {
            mutex.unlock();
            ret = (*tmp)->setVar(name, data);
            mutex.lock();
        }
        return ret == VarSpace::vof_success;
    }
    
    /**
     * 删除变量
     * 若not_var则跳到下一个变量空间
     * 若fail则结束
     * @param name
     * @return
     */
    bool VarList::delVar(const std::string &name){
        std::unique_lock<std::mutex> mutex{lock};
        VarSpace::VarOperationFlat ret = VarSpace::vof_not_var;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_not_var; tmp++) {
            mutex.unlock();
            ret = (*tmp)->delVar(name);
            mutex.lock();
        }
        return ret == VarSpace::vof_success;
    }
    
    void VarList::connect(VarList *varlist){
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &t: varlist->varspace) {
            mutex.unlock();
            this->varspace.push_back(t);
            mutex.lock();
        }
    }

}