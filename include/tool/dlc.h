#ifndef AFUN_DLC_H
#define AFUN_DLC_H
#include <iostream>
#include "aFunToolExport.h"
#include "macro.h"
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

    /**
     * DlcHandle: 动态库句柄
     * 注意: 仅能通过 openLibrary生成
     * 不需要 delete 释放 (自动管理释放)
     */
    class AFUN_TOOL_EXPORT DlcHandle {
    public:
        DlcHandle(const char *file, int mode) noexcept;  // 仅 openLibary 可用
        AFUN_INLINE DlcHandle(const DlcHandle &dlc_handle) noexcept;
        AFUN_INLINE DlcHandle(DlcHandle &&dlc_handle) noexcept;
        AFUN_INLINE ~DlcHandle() noexcept;
        AFUN_INLINE DlcHandle &operator=(const DlcHandle &dlc_handle) noexcept;

        [[nodiscard]] AFUN_INLINE bool isOpen() const;

        /**
         * 获得动态库中指定名字的符号
         * @tparam SYMBOL 符号类型
         * @param name 名字
         * @return 符号
         */
        template<typename SYMBOL>
        DlcSymbol<SYMBOL> getSymbol(const std::string &name);

        /**
         * 关闭动态库句柄
         */
        AFUN_INLINE void close();
        AFUN_INLINE int operator++(int);
        AFUN_INLINE int operator--(int);

        AFUN_STATIC void dlcExit();

        class AFUN_TOOL_EXPORT Handle {
            friend class DlcHandle;
        public:
            explicit Handle(void *handle);  // 仅 openLibary 可用
            Handle(const Handle &dlc) = delete;
            Handle &operator=(const Handle *dlc) = delete;
            ~Handle();

            [[nodiscard]] AFUN_INLINE bool isOpen() const;

            /**
             * 获得动态库中指定名字的符号
             * @tparam SYMBOL 符号类型
             * @param name 名字
             * @return 符号
             */
            template<typename SYMBOL>
            DlcSymbol<SYMBOL> getSymbol(const std::string &name);

            int operator++(int);
            int operator--(int);

        private:
            void *handle_;
            int link_;  // 引用计数
            class Handle *next_;
            class Handle *prev_;
        };

    private:
        class Handle *handle_;
        AFUN_STATIC Handle *dlc;
    };

}

#include "dlc.inline.h"
#include "dlc.template.h"

#endif //AFUN_DLC_H
