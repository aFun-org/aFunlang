#include "tool.h"
#include "dlc.h"
using namespace aFuntool;

DlcHandle::Handle *DlcHandle::dlc = nullptr;

/**
 * 打开动态库
 * @param file 动态库路径
 * @param mode 模式
 * @return
 */
aFuntool::DlcHandle::DlcHandle(const char *file, int mode) noexcept : handle_{nullptr} {
    void *handle = dlopen(file, mode);
    if (handle == nullptr)
        return;

    for (Handle *tmp = dlc; tmp != nullptr; tmp = tmp->next_) {
        if (tmp->handle_ == handle) {
            dlclose(handle);  // 减少dlopen时对handle的引用计数
            (*tmp)++;
            handle_ = tmp;
            return;
        }
    }

    handle_ = new Handle(handle);
    (*handle_)++;
}

aFuntool::DlcHandle::Handle::Handle(void *handle) : handle_{handle}, link_{0}, next_{DlcHandle::dlc}, prev_{nullptr} {
    if (DlcHandle::dlc != nullptr)
        DlcHandle::dlc->prev_ = dlc;
    DlcHandle::dlc = this;
}


aFuntool::DlcHandle::Handle::~Handle() {
    dlclose(handle_);

    if (prev_ == nullptr)
        dlc = next_;
    else
        prev_->next_ = next_;

    if (next_ != nullptr)
        next_->prev_ = prev_;
}


int aFuntool::DlcHandle::Handle::operator++(int){
    return link_++;
}


int aFuntool::DlcHandle::Handle::operator--(int){
    int ret = link_--;
    if (link_ == 0)
        delete this;  // 删除自己
    return ret;
}

/**
 * 退出函数
 * 需要使用at_exit注册
 */
void aFuntool::DlcHandle::dlcExit() {
    while (dlc != nullptr) {
        auto next = dlc->next_;
        free(dlc);
        dlc = next;
    }
}
