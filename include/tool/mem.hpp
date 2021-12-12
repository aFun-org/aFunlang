/*
 * 文件名: mem.h
 * 目标: 内存管理工具的头文件
 */

#ifndef AFUN_MEM_HPP
#define AFUN_MEM_HPP

#include <cstdlib>
#include "log.hpp"


/* 取代calloc函数 */
namespace aFuntool {
    template <typename T>
    static void *safeFree(T *&ptr) {if (ptr != nullptr) free((void *)ptr); ptr = nullptr; return nullptr;}

    static void *safeCalloc(size_t n, size_t size){
        void *re = calloc(n, size);
        if (re == nullptr)
            fatalErrorLog(nullptr, EXIT_FAILURE, "The memory error");
        return re;
    }

    template <typename T>
    static void *safeCalloc(size_t n, T &t){
        void *re = calloc(n, sizeof(decltype(*t)));  // 自动推断类型
        if (re == nullptr)
            fatalErrorLog(nullptr, EXIT_FAILURE, "The memory error");
        return re;
    }
}

#define calloc(n, obj) (obj *)(aFuntool::safeCalloc(n, sizeof(obj)))
#define calloc_size(n, size) (aFuntool::safeCalloc(n, size))

#endif  // AFUN_MEM_HPP