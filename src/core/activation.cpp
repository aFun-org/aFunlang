#include "activation.hpp"
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

TopActivation::TopActivation(Code *code, Inter *inter_)
    : Activation(inter_), start{code}, next{code} {
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

ActivationStatus TopActivation::getCode(Code *&code) {
    code = next;
    if (code == nullptr)
        return as_end;

    if (code->getType() != code_start) {
        Message *msg = down->getMessage<NormalMessage>("NORMAL");
        if (msg == nullptr) {
            return as_end;
        } else
            msg = down->popMessage("NORMAL");
        delete msg;
    }

    next = code->toNext();
    return as_run;
}
