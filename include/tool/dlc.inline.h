#ifndef AFUN_DLC_INLINE_H
#define AFUN_DLC_INLINE_H

#include "dlc.h"

namespace aFuntool {
    template<typename SYMBOL>
    DlcSymbol<SYMBOL> DlcHandle::getSymbol(const std::string &name){
        return handle_ != nullptr ? handle_->getSymbol<SYMBOL>(name) : DlcSymbol<SYMBOL>();
    }

    DlcHandle::~DlcHandle() noexcept {
        this->close();
    }

    bool DlcHandle::isOpen() const {
        return handle_ != nullptr ? handle_->isOpen() : false;
    }

    void DlcHandle::close(){
        if (handle_ == nullptr)
            return;
        (*handle_)--;
        handle_ = nullptr;
    }

    int DlcHandle::operator++(int){
        return (*handle_)++;
    }

    int DlcHandle::operator--(int){
        return (*handle_)--;
    }

    DlcHandle &DlcHandle::operator=(const DlcHandle &dlc_handle) noexcept {
        if (&dlc_handle == this)
            return *this;

        this->close();
        handle_ = dlc_handle.handle_;
        if (handle_ != nullptr)
            (*handle_)++;
        return *this;
    }

    DlcHandle::DlcHandle(const DlcHandle &dlc_handle) noexcept {
        handle_ = dlc_handle.handle_;
        if (handle_ != nullptr)
            (*handle_)++;
    }

    DlcHandle::DlcHandle(DlcHandle &&dlc_handle)noexcept {
        handle_ = dlc_handle.handle_;
        dlc_handle.handle_ = nullptr;
    }

    template<typename SYMBOL>
    DlcSymbol<SYMBOL> DlcHandle::Handle::getSymbol(const std::string &name){
        if (handle_ == nullptr)
            return DlcSymbol<SYMBOL>();
        auto symbol = (SYMBOL *)dlsym(handle_, name.c_str());
        if (symbol == nullptr)
            return DlcSymbol<SYMBOL>();
        return DlcSymbol<SYMBOL>(symbol, this);
    }

    bool DlcHandle::Handle::isOpen() const {
        return handle_ != nullptr;
    }
}

#endif //AFUN_DLC_INLINE_H
