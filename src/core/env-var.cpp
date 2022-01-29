#include "env-var.h"
namespace aFuncore {
    /**
     * 获取环境变量文本
     * @param name 变量名
     * @param str 文本
     * @return 是否成功
     */
    bool EnvVarSpace::findString(const std::string &name, std::string &str) const{
        try {
            auto &env_var = var.at(name);
            str = env_var.str;
            return true;
        } catch (std::out_of_range &) {
            return false;
        }
    }

    /**
     * 获取环境变量数值
     * @param name 变量名
     * @param num 文本
     * @return 是否成功
     */
    bool EnvVarSpace::findNumber(const std::string &name, int32_t &num) const{
        try {
            auto &env_var = var.at(name);
            num = env_var.num;
            return true;
        } catch (std::out_of_range &) {
            return false;
        }
    }

    /**
     * 设置环境变量文本
     * @param name 变量名
     * @param str 文本
     */
    void EnvVarSpace::setString(const std::string &name, const std::string &str){
        try {
            auto &env_var = var.at(name);
            env_var.str = str;
        } catch (std::out_of_range &) {
            var.insert({name, {str, 0}});
        }
    }

    /**
     * 设置环境变量数值
     * @param name 变量名
     * @param num 数值
     */
    void EnvVarSpace::setNumber(const std::string &name, int32_t num){
        try {
            auto &env_var = var.at(name);
            env_var.num = num;
        } catch (std::out_of_range &) {
            var.insert({name, {"", num}});
        }
    }

    /**
     * 设置环境变量文本 （加法）
     * @param name 变量名
     * @param str 文本
     */
    void EnvVarSpace::addString(const std::string &name, const std::string &str){
        try {
            auto &env_var = var.at(name);
            env_var.str += str;
        } catch (std::out_of_range &) {
            var.insert({name, {str, 0}});
        }
    }

    /**
     * 设置环境变量数值 （加法）
     * @param name 变量名
     * @param num 数值
     */
    void EnvVarSpace::addNumber(const std::string &name, int32_t num){
        try {
            auto &env_var = var.at(name);
            env_var.num += num;
        } catch (std::out_of_range &) {
            var.insert({name, {"", num}});
        }
    }
}