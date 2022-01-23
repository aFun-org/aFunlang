//
// Created by jimso on 2022/1/8.
//

#ifndef AFUN_DLC_TEMPLATE_H
#define AFUN_DLC_TEMPLATE_H

#include "dlc.h"

namespace aFuntool {
    /**
     * 符号句柄
     * 注意: 不适用符号后需要 delete
     * @tparam SYMBOL 符号类型
     */
    template <typename SYMBOL>
    class DlcSymbol {
    public:
        DlcSymbol() noexcept = default;

        /**
         * 从句柄和符号指针创建一个符号
         * @param symbol 符号指针
         * @param dlc 句柄
         */
        explicit DlcSymbol(SYMBOL *symbol_, DlcHandle::Handle *dlc_) noexcept : symbol_ {symbol_}, handle_ {dlc_} {
            if (this->handle_ != nullptr)
                (*handle_)++;
        }

        DlcSymbol(const DlcSymbol &dlc_symbol) noexcept : symbol_{dlc_symbol.symbol_}, handle_ {dlc_symbol.handle_} {
            if (handle_ != nullptr)
                (*handle_)++;
        }

        DlcSymbol(DlcSymbol &&dlc_symbol) noexcept : symbol_{dlc_symbol.symbol_}, handle_ {dlc_symbol.handle_} {
            dlc_symbol.handle_ = nullptr;
        }

        DlcSymbol &operator=(const DlcSymbol &dlc_symbol) noexcept {
            if (this == &dlc_symbol)
                return *this;

            if (handle_ != nullptr)
                (*handle_)--;
            symbol_ = dlc_symbol.symbol_;
            handle_ = dlc_symbol.handle_;
            if (handle_ != nullptr)
                (*handle_)++;
            return *this;
        }

        ~DlcSymbol() noexcept {
            if (handle_ != nullptr)
                (*handle_)--;
        }

        SYMBOL *getSymbol() const {
            return symbol_;
        }

    private:
        SYMBOL *symbol_ = nullptr;
        DlcHandle::Handle *handle_ = nullptr;
    };
}

#endif //AFUN_DLC_TEMPLATE_H
