#include "rt_varspace_object.h"

namespace aFunrt {
    VarSpace::VarSpace(aFuncore::Inter &inter) : Object("VarSpace", inter), env{inter.getEnvironment()}{

    }

    VarSpace::VarSpace(aFuncore::Environment &env_) : Object("VarSpace", env_), env{env_}{

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

    void VarSpace::linkObject(std::queue<Object *> &queue) {
        for (auto tmp : var)
            queue.push(tmp.second);
    }
}