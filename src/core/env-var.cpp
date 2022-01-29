#include "env-var.h"
namespace aFuncore {
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

    /**
     * 设置环境变量文本
     * @param name 变量名
     * @param str 文本
     */
    void EnvVarSpace::setString(const std::string &name, const std::string &str){
        std::unique_lock<std::mutex> mutex{lock};
        auto env_var = var.find(name);
        if (env_var == var.end())
            var.insert({name, {str, 0}});
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
            var.insert({name, {"", num}});
        else
            env_var->second.num = num;
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
            var.insert({name, {str, 0}});
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
            var.insert({name, {"", num}});
        else
            env_var->second.num += num;
    }
}