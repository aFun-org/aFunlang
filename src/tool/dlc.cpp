#include "tool.hpp"
#include "dlc.hpp"
using namespace aFuntool;

static DlcHandle *dlc_l = nullptr;

/**
 * 打开动态库
 * @param file 动态库路径
 * @param mode 模式
 * @return
 */
DlcHandle *aFuntool::openLibrary(const char *file, int mode) {
    void *handle = dlopen(file, mode);
    DlcHandle *dlc;

    if (handle == nullptr)
        return nullptr;

    for (struct DlcHandle *tmp = dlc_l; tmp != nullptr; tmp = tmp->next) {
        if (tmp->handle == handle) {
            dlclose(handle);  // 减少dlopen时对handle的引用计数
            tmp++;
            return tmp;
        }
    }

    dlc = new DlcHandle(handle);

    dlc->next = dlc_l;
    dlc->prev = nullptr;
    if (dlc_l != nullptr)
        dlc_l->prev = dlc;
    dlc_l = dlc;

    return dlc;
}

aFuntool::DlcHandle::DlcHandle(void *handle){
    this->handle = handle;
    this->link = 1;
    this->next = nullptr;
    this->prev = nullptr;
}


aFuntool::DlcHandle::~DlcHandle() {
    dlclose(handle);

    if (prev == nullptr)
        dlc_l = next;
    else
        prev->next = next;

    if (next != nullptr)
        next->prev = prev;
}


void aFuntool::DlcHandle::close() {
    this->operator--(1);
}


int aFuntool::DlcHandle::operator++(int){
    return link++;
}


int aFuntool::DlcHandle::operator--(int){
    int ret = link--;
    if (link == 0)
        delete this;  // 删除自己
    return ret;
}

/**
 * 退出函数
 * 需要使用at_exit注册
 */
void aFuntool::dlcExit() {
    while (dlc_l != nullptr) {
        auto next = dlc_l->next;
        free(dlc_l);
        dlc_l = next;
    }
}
