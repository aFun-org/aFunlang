#include "activation.hpp"
#include "value.hpp"
#include "inter.hpp"
#include "init.hpp"
#include "msg.hpp"
#include "var.hpp"
#include "code.hpp"

using namespace aFuncore;
using namespace aFuntool;

/**
 * 创建基本Activation
 * 若上层Activation已到结尾则尾调用优化
 * 自动继承上层VarList和UpMessage
 * 自动压入inter
 * @param inter_
 */
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

/**
 * 析构Activation
 * 注意: 不会自动从inter中弹出
 * 释放Varlist并且将DownMessage压入上层
 */
Activation::~Activation(){
    if (varlist != nullptr && old_varlist != nullptr)
        varlist->disconnect(old_varlist);
    if (prev && down != nullptr)
        down->joinMsg(prev->down);
    delete up;
    delete down;
}

/**
 * 运行代码
 * @param code
 */
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
                trackLog(aFunCoreLogger, "Find Var %s -> %p", code->getElement(), obj);
                if (obj != nullptr)
                    down->pushMessage(new NormalMessage(obj));
            }
        } else switch (code->getBlockType()) {
            case block_p:  // 顺序执行
                new ExeActivation(code->getSon(), inter);
                break;
            case block_b:
            case block_c:
                new FuncActivation(code, inter);
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
        auto msg = down->getMessage<NormalMessage>("NORMAL");
        if (msg == nullptr)
            return as_end;
        else
            down->popMessage("NORMAL");
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

FuncActivation::~FuncActivation(){
    delete call_func;
}

ActivationStatus FuncActivation::getCode(Code *&code){
    if (on_tail)
        return as_end;

    if (status == func_first) {
        status = func_get_func;
        switch (call->getBlockType()) {
            case block_c:
                code = call->getSon();
                return as_run;
            case block_b:
                break;
            default:
                errorLog(aFunCoreLogger, "Error FuncActivation block type");
                return as_end;
        }
    }

    if (status == func_get_func) {
        status = func_get_arg;
        auto *msg = down->getMessage<NormalMessage>("NORMAL");
        if (msg == nullptr)
            return as_end;
        else
            down->popMessage("NORMAL");
        func = dynamic_cast<Function *>(msg->getObject());
        delete msg;
        if (func == nullptr)
            return as_end;

        call_func = func->getCallFunction(call, inter);
        acl = call_func->getArgCodeList();
        acl_begin = acl->begin();
        acl_end = acl->end();
        if (acl_begin != acl_end) {
            code = acl_begin->code;
            return as_run;
        }
    }

    auto *msg = down->getMessage<NormalMessage>("NORMAL");
    if (msg == nullptr)
        return as_end;
    else
        down->popMessage("NORMAL");

    acl_begin->ret = msg->getObject();
    delete msg;

    acl_begin++;
    if (acl_begin != acl_end) {
        code = acl_begin->code;
        return as_run;
    }

    on_tail = true;
    return call_func->runFunction();
}
