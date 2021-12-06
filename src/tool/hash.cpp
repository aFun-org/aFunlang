/*
 * 文件名: hash.c
 * 目标: 关于哈希表的实用函数
 */

#include "tool.hpp"

time33_t time33(const char *str) {
    unsigned int hash = 5381;
    while(*str)
        hash += (hash << 5 ) + (*str++);
    return (hash & 0x7FFFFFFF);
}
