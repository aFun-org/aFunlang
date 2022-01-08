#include "activation.h"
#include "value.h"
#include "inter.h"
#include "init.h"
#include "msg.h"
#include "var.h"
#include "code.h"
#include "env-var.h"

using namespace aFuncore;
using namespace aFuntool;

/**
 * 创建基本Activation
 * 自动继承上层VarList和UpMessage
 * 自动压入inter
 * @param inter_
 */
Activation::Activation(Inter *inter_) : inter{inter_}, line{0} {
    Activation *prev_ = inter->getActivation();
    prev = prev_;
    down = new DownMessage();
    if (prev != nullptr) {
        varlist = new VarList(prev->varlist);
        up = new UpMessage(prev->up);
        line = prev->line;
        path = prev->path;
    } else {
        varlist = new VarList();
        up = new UpMessage();
        line = 0;
        path = "";
    }
    inter->pushActivation(this);
}

/**
 * 析构Activation
 * 注意: 不会自动从inter中弹出
 * 释放Varlist并且将DownMessage压入上层
 */
Activation::~Activation(){
    if (prev != nullptr && down != nullptr)
        down->joinMsg(prev->down);
    delete up;
    delete down;
    delete varlist;
}

/**
 * 运行代码
 * @param code
 */
void Activation::runCode(Code *code) {
    auto code_type = code->getType();
    if (code_type == code_start) {  // start 不处理 msg
        auto *none = new Object("None", inter);
        down->pushMessage(new NormalMessage(none));
    } else {
        if (code_type == code_element) {
            runCodeElement(code);
        } else switch (code->getBlockType()) {
            case block_p:  // 顺序执行
                runCodeBlockP(code);
                break;
            case block_b:
                runCodeBlockB(code);
                break;
            case block_c:
                runCodeBlockC(code);
                break;
            default:
                errorLog(aFunCoreLogger, "Error block type.");
                break;
        }
    }
}

void Activation::runCodeElement(Code *code) {
    std::string literaler_name;
    bool in_protect = false;
    Object *obj = nullptr;
    if (inter->checkLiteral(code->getElement(), literaler_name, in_protect)) {
        if (in_protect)
            obj = inter->getProtectVarSpace()->findObject(literaler_name);
        else
            obj = varlist->findObject(literaler_name);
        auto literaler = dynamic_cast<Literaler *>(obj);
        if (literaler != nullptr)
            literaler->getObject(code->getElement(), code->getPrefix());
        else
            down->pushMessage(new ErrorMessage("TypeError", "Error type of literal.", this));
    } else {
        if (varlist != nullptr)
            obj = varlist->findObject(code->getElement());
        if (obj != nullptr) {
            auto cbv = dynamic_cast<CallBackVar *>(obj);
            if (cbv != nullptr && cbv->isCallBack())
                cbv->callBack();
            else
                down->pushMessage(new NormalMessage(obj));
        } else
            down->pushMessage(new ErrorMessage("NameError", std::string("Variable ") + code->getElement() + " not fount.", this));
    }
}

void Activation::runCodeBlockP(Code *code) {
    new ExeActivation(code->getSon(), inter);
}

void Activation::runCodeBlockC(Code *code) {
    new FuncActivation(code, inter);
}

void Activation::runCodeBlockB(Code *code) {
    new FuncActivation(code, inter);
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
    line = code->getFileLine();
    if (code->getFilePath() != nullptr)
        path = code->getFilePath();
    next = code->toNext();
    return as_run;
}

TopActivation::TopActivation(Code *code, Inter *inter_) : ExeActivation(code, inter_) {
    varlist->connect(inter_->getGlobalVarlist());
}

static void ActivationTopProgress(Message *msg, void *) {
    auto *t = dynamic_cast<TopMessage *>(msg);
    if (t)
        t->topProgress();
};

TopActivation::~TopActivation() {
    down->forEach(ActivationTopProgress, nullptr);
}

FuncActivation::~FuncActivation(){
    delete call_func;
}

ActivationStatus FuncActivation::getCode(Code *&code){
    if (on_tail)
        return as_end;

    if (status == func_first) {
        switch (call->getBlockType()) {
            case block_c:
                status = func_get_func;
                code = call->getSon();
                if (code == nullptr) {
                    line = 0;
                    down->pushMessage(new ErrorMessage("SyntaxError", "Callback without code.", this));
                    return as_end;
                }
                line = code->getFileLine();
                if (code->getFilePath() != nullptr)
                    path = code->getFilePath();
                return as_run;
            case block_b: {
                std::string prefix;
                if (!inter->getEnvVarSpace()->findString("sys:prefix", prefix) || prefix.size() != PREFIX_COUNT)
                    prefix = "''";
                char quote = prefix[prefix_quote];
                for (Code *var = call->getSon(); var != nullptr; var = var->toNext()) {
                    if (var->getType() != code_element || var->getPrefix() == quote || inter->checkLiteral(var->getElement()))
                        continue;
                    Object *obj = varlist->findObject(var->getElement());
                    if (obj == nullptr || !dynamic_cast<Function *>(obj) || !dynamic_cast<Function *>(obj)->isInfix())
                        continue;
                    func = dynamic_cast<Function *>(obj);
                    if (func == nullptr || !func->isInfix())
                        continue;
                    status = func_get_func;
                    break;  /* 跳转到: 执行变量获取前的准备 */
                }
                if (status != func_get_func) {
                    line = 0;
                    down->pushMessage(new ErrorMessage("SyntaxError", "Callback without code.", this));
                    return as_end;
                }
                break;
            }
            default:
                errorLog(aFunCoreLogger, "Error FuncActivation block type");
                line = 0;
                down->pushMessage(new ErrorMessage("RuntimeError", "Error FuncActivation block type.", this));
                return as_end;
        }
    }

    if (status == func_get_func) {
        if (func == nullptr) {
            auto *msg = down->getMessage<NormalMessage>("NORMAL");
            if (msg == nullptr)
                return as_end;
            else
                down->popMessage("NORMAL");
            func = dynamic_cast<Function *>(msg->getObject());
            delete msg;
            if (func == nullptr) {
                down->pushMessage(new ErrorMessage("TypeError", "Callback without function.", this));
                return as_end;
            }
        }

        /* Label: 执行变量获取前的准备 */
        status = func_get_arg;
        call_func = func->getCallFunction(call, inter);
        acl = call_func->getArgCodeList();
        acl_begin = acl->begin();
        acl_end = acl->end();
        if (acl_begin != acl_end) {  // 如果有参数需要计算
            code = acl_begin->code;
            line = code->getFileLine();
            if (code->getFilePath() != nullptr)
                path = code->getFilePath();
            return as_run;
        }
    }

    if (acl_begin != acl_end) {  // 获取参数计算结果
        auto *msg = down->getMessage<NormalMessage>("NORMAL");
        if (msg == nullptr)
            return as_end;
        down->popMessage("NORMAL");

        acl_begin->ret = msg->getObject();
        delete msg;

        acl_begin++;
        if (acl_begin != acl_end) {
            code = acl_begin->code;
            line = code->getFileLine();
            if (code->getFilePath() != nullptr)
                path = code->getFilePath();
            return as_run;
        }
    }

    on_tail = true;
    line = 0;
    return as_end_run;
}

void FuncActivation::endRun(){
    call_func->runFunction();
}
