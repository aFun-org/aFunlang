#include "activation.hpp"
#include "value.hpp"
#include "init.hpp"

using namespace aFuncore;
using namespace aFuntool;

Activation::Activation(Inter *inter_) : inter{inter_}, line{0} {
    Activation *prev_ = inter->getActivation();
    if (prev_ != nullptr && prev_->onTail()) {
        prev = prev_->prev;
        up = prev_->up;
        down = prev_->down;
        old_varlist = prev_->old_varlist;
        varlist = prev_->varlist;

        prev_->up = nullptr;
        prev_->down = nullptr;
        prev_->old_varlist = nullptr;
        prev_->varlist = nullptr;
        delete prev_;
    } else {
        prev = prev_;
        old_varlist = prev ? prev->varlist : nullptr;
        varlist = old_varlist;
        down = new DownMessage();
        up = new UpMessage(prev ? prev->up : nullptr);
    }
    inter->pushActivation(this);
}

Activation::~Activation(){
    if (varlist != nullptr && old_varlist != nullptr)
        varlist->disconnect(old_varlist);
    if (prev && down != nullptr)
        down->joinMsg(prev->down);
    delete up;
    delete down;
}

void Activation::runCode(Code *code){
    auto code_type = code->getType();
    if (code_type == code_start) {  // start 不处理 msg
        auto *none = new Object("None", inter);
        down->pushMessage(new NormalMessage(none));
    } else {
        if (code_type == code_element) {
            std::string func;
            bool in_protect = false;
            if (inter->checkLiteral(code->getElement(), func, in_protect)) {
                // ...
            } else {
                Object *obj = nullptr;
                if (varlist != nullptr)
                    obj = varlist->findObject(code->getElement());
                if (obj != nullptr)
                    down->pushMessage(new NormalMessage(obj));
            }
        } else switch (code->getBlockType()) {
            case block_p:  // 顺序执行
                new ExeActivation(code->getSon(), inter);
                break;
            case block_b:
                break;
            case block_c:
                break;
            default:
                errorLog(aFunCoreLogger, "Error block type.");
                break;
        }
    }
}

ActivationStatus ExeActivation::getCode(Code *&code){
    code = next;
    if (code == nullptr)
        return as_end;

    if (!first) {
        Message *msg = down->getMessage<NormalMessage>("NORMAL");
        if (msg == nullptr) {
            return as_end;
        } else
            msg = down->popMessage("NORMAL");
        delete msg;
    }

    first = false;
    next = code->toNext();
    return as_run;
}

TopActivation::TopActivation(Code *code, Inter *inter_) : ExeActivation(code, inter_) {
    varlist = inter_->getGlobalVarlist();
    old_varlist = varlist;
}

static void ActivationTopProgress(Message *msg, void *) {
    auto *t = dynamic_cast<TopMessage *>(msg);
    if (t)
        t->topProgress();
};

TopActivation::~TopActivation() {
    down->forEach<void *>(ActivationTopProgress, nullptr);
}
