/*
 * 文件名: hash.c
 * 目标: 关于哈希表的实用函数
 */

#include "tool.hpp"
using namespace aFuntool;

time33_t aFuntool::time33(const char *str) {
    unsigned int hash = 5381;
    while(*str)
        hash += (hash << 5 ) + (*str++);
    return (hash & 0x7FFFFFFF);  // NOLINT
}

time33_t aFuntool::time33(const std::string &str) {
    unsigned int hash = 5381;
    for (auto ch : str)
        hash += (hash << 5 ) + ch;
    return (hash & 0x7FFFFFFF);  // NOLINT
}
