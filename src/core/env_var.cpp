#include "core_env_var.h"
#include "core_object.h"

namespace aFuncore {
    EnvVarSpace::~EnvVarSpace() {
        for (auto &i : var) {
            if (i.second.object != nullptr)
                i.second.object->delReference();
        }
    }

    /**
     * 获取环境变量文本
     * @param name 变量名
     * @param str 文本
     * @return 是否成功
     */
    bool EnvVarSpace::findString(const std::string &name, std::string &str) {
        std::unique_lock<std::mutex> mutex{lock};
        auto env_var = var.find(name);
        if (env_var == var.end())
            return false;
        str = env_var->second.str;
        return true;
    }

    /**
     * 获取环境变量数值
     * @param name 变量名
     * @param num 文本
     * @return 是否成功
     */
    bool EnvVarSpace::findNumber(const std::string &name, int32_t &num) {
        std::unique_lock<std::mutex> mutex{lock};
        auto env_var = var.find(name);
        if (env_var == var.end())
            return false;
        num = env_var->second.num;
        return true;
    }

    bool EnvVarSpace::findObject(const std::string &name, Object *&obj) {
        std::unique_lock<std::mutex> mutex{lock};
        auto env_var = var.find(name);
        if (env_var == var.end())
            return false;
        obj = env_var->second.object;
        return true;
    }

    /**
     * 设置环境变量文本
     * @param name 变量名
     * @param str 文本
     */
    void EnvVarSpace::setString(const std::string &name, const std::string &str){
        std::unique_lock<std::mutex> mutex{lock};
        auto env_var = var.find(name);
        if (env_var == var.end())
            var.insert({name, {str, 0, nullptr}});
        else
            env_var->second.str = str;
    }

    /**
     * 设置环境变量数值
     * @param name 变量名
     * @param num 数值
     */
    void EnvVarSpace::setNumber(const std::string &name, int32_t num){
        std::unique_lock<std::mutex> mutex{lock};
        auto env_var = var.find(name);
        if (env_var == var.end())
            var.insert({name, {"", num, nullptr}});
        else
            env_var->second.num = num;
    }

    void EnvVarSpace::setObject(const std::string &name, Object *obj) {
        std::unique_lock<std::mutex> mutex{lock};
        obj->addReference();
        auto env_var = var.find(name);
        if (env_var == var.end())
            var.insert({name, {"", 0, obj}});
        else {
            if (env_var->second.object != nullptr)
                env_var->second.object->delReference();
            env_var->second.object = obj;
        }
    }

    /**
     * 设置环境变量文本 （加法）
     * @param name 变量名
     * @param str 文本
     */
    void EnvVarSpace::addString(const std::string &name, const std::string &str){
        std::unique_lock<std::mutex> mutex{lock};
        auto env_var = var.find(name);
        if (env_var == var.end())
            var.insert({name, {str, 0, nullptr}});
        else
            env_var->second.str += str;
    }

    /**
     * 设置环境变量数值 （加法）
     * @param name 变量名
     * @param num 数值
     */
    void EnvVarSpace::addNumber(const std::string &name, int32_t num){
        std::unique_lock<std::mutex> mutex{lock};
        auto env_var = var.find(name);
        if (env_var == var.end())
            var.insert({name, {"", num, nullptr}});
        else
            env_var->second.num += num;
    }
}