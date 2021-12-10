﻿#ifndef AFUN_DLC_HPP
#define AFUN_DLC_HPP
#include <iostream>
#include "aFunToolExport.h"
#include "dlfcn.h"  // CMake 处理 dlfcn.h 的位置


/* 动态库工具(dlc): 处理动态库的使用 */

/*
 * NEW_DLC_SYMBOL: 用于定义指定类型的symbol结构体
 * DLC_SYMBOL: 指定类型的symbol结构体名
 * GET_SYMBOL: 访问symbol成员的值
 * MAKE_SYMBOL: 生成一个symbol
 * COPY_SYMBOL: 拷贝一个symbol(拷贝其引用)
 * READ_SYMBOL: 在dlc中获取一个symbol
 * FREE_SYMBOL： 释放symbol
 *
 * openLibary: 打开动态库
 * freeLibary: 释放动态库
 * dlcExit: 释放所有动态库
 */

namespace aFuntool {
    class DlcHandle;

    template <typename SYMBOL>
    class DlcSymbol;

    AFUN_TOOL_EXPORT DlcHandle *openLibrary(const char *file, int mode);
    AFUN_TOOL_EXPORT void dlcExit();

    /**
     * DlcHandle: 动态库句柄
     * 注意: 仅能通过 openLibrary生成
     * 不需要 delete 释放 (自动管理释放)
     */
    class DlcHandle {
        friend void dlcExit();
        friend DlcHandle *openLibrary(const char *file, int mode);

        explicit DlcHandle(void *handle);  // 仅 openLibary 可用
        void *handle;
        int link;  // 引用计数
        struct DlcHandle *next;
        struct DlcHandle *prev;
    public:
        ~DlcHandle();

        /**
         * 获得动态库中指定名字的符号
         * @tparam SYMBOL 符号类型
         * @param name 名字
         * @return 符号
         */
        template<typename SYMBOL>
        DlcSymbol<SYMBOL> *get_symbol(const std::string &name) {
            auto symbol = (SYMBOL *)dlsym(handle, name.c_str());
            if (symbol == nullptr)
                return nullptr;
            return new DlcSymbol<SYMBOL>(symbol, this);
        }

        /**
         * 关闭动态库句柄
         */
        void close();
        int operator++(int);
        int operator--(int);
    };

    /**
     * 符号句柄
     * 注意: 不适用符号后需要 delete
     * @tparam SYMBOL 符号类型
     */
    template <typename SYMBOL>
    class DlcSymbol {
        SYMBOL *symbol;
        class DlcHandle *dlc = nullptr;

    public:
        /**
         * 从句柄和符号指针创建一个符号
         * @param symbol 符号指针
         * @param dlc 句柄
         */
        explicit DlcSymbol(SYMBOL *symbol, class DlcHandle *dlc) {
            this->symbol = symbol;
            this->dlc = dlc;
            if (this->dlc != nullptr)
                this->dlc++;
        }

        /**
         * 复制符号
         * @param symbol
         */
        explicit DlcSymbol(class DlcSymbol<SYMBOL> *symbol) {
            this->symbol = symbol->symbol;
            this->dlc = symbol->dlc;
            if (this->dlc != nullptr)
                this->dlc++;
        }

        ~DlcSymbol(){
            if (dlc != nullptr)
                dlc--;
        }

        SYMBOL *getSymbol() const {
            return symbol;
        }
    };
}

#endif //AFUN_DLC_HPP
