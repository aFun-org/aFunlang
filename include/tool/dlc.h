#ifndef AFUN_DLC_H
#define AFUN_DLC_H
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

    AFUN_TOOL_EXPORT void dlcExit();
    AFUN_TOOL_EXPORT DlcHandle *openLibrary(const char *file, int mode);

    /**
     * DlcHandle: 动态库句柄
     * 注意: 仅能通过 openLibrary生成
     * 不需要 delete 释放 (自动管理释放)
     */
    class AFUN_TOOL_EXPORT DlcHandle {
        friend AFUN_TOOL_EXPORT void dlcExit();
        friend AFUN_TOOL_EXPORT DlcHandle *openLibrary(const char *file, int mode);

        explicit DlcHandle(void *handle);  // 仅 openLibary 可用
        void *handle;
        int link;  // 引用计数
        struct DlcHandle *next;
        struct DlcHandle *prev;
    public:
        DlcHandle(const DlcHandle &dlc)=delete;
        DlcHandle &operator=(const DlcHandle *dlc)=delete;
        ~DlcHandle();

        /**
         * 获得动态库中指定名字的符号
         * @tparam SYMBOL 符号类型
         * @param name 名字
         * @return 符号
         */
        template<typename SYMBOL>
        DlcSymbol<SYMBOL> *get_symbol(const std::string &name);

        /**
         * 关闭动态库句柄
         */
        void close();
        int operator++(int);
        int operator--(int);
    };

}

#include "dlc.inline.h"
#include "dlc.template.h"

#endif //AFUN_DLC_H
