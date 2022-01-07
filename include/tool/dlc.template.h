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
        const SYMBOL *symbol;
        const DlcHandle *dlc = nullptr;
    public:
        /**
         * 从句柄和符号指针创建一个符号
         * @param symbol 符号指针
         * @param dlc 句柄
         */
        explicit DlcSymbol(SYMBOL *symbol_, class DlcHandle *dlc_) : symbol {symbol_}, dlc {dlc_} {
            if (this->dlc != nullptr)
                this->dlc++;
        }

        DlcSymbol(const DlcSymbol &dlc_symbol) : symbol{dlc_symbol.symbol}, dlc {dlc_symbol.dlc} {
            if (this->dlc != nullptr)
                this->dlc++;
        }

        DlcSymbol &operator=(const DlcSymbol &dlc_symbol) {
            if (this == &dlc_symbol)
                return *this;
            if (this->dlc != nullptr)
                this->dlc--;
            symbol = dlc_symbol.symbol;
            dlc = dlc_symbol.dlc;
            if (this->dlc != nullptr)
                this->dlc++;
            return *this;
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

        const SYMBOL *getSymbol() const {
            return symbol;
        }
    };
}

#endif //AFUN_DLC_TEMPLATE_H
