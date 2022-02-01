#include "varlist.h"
#include "object-value.h"
#include "inter.h"
#include "init.h"

namespace aFuncore {
    VarList::VarList(VarList *varlist){
        std::unique_lock<std::mutex> mutex{lock};
        for (auto &t: varlist->varspace)
            this->varspace.push_back(t);
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