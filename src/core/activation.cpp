#include "core-activation.h"
#include "inter.h"
#include "core-init.h"
#include "msg.h"
#include "code.h"
#include "core-exception.h"

namespace aFuncore {
    /**
     * 创建基本Activation
     * 自动继承上层VarList和UpMessage
     * 自动压入inter
     * @param inter_
     */
    Activation::Activation(Inter &inter_)
            : inter{inter_}, up{inter_.activation == nullptr ? nullptr : &inter_.activation->up}, down{}, line{0} {
        if (inter_.activation != nullptr) {
            varlist.connect(inter_.activation->varlist);
            line = inter_.activation->line;
            path = inter_.activation->path;
        } else {
            auto global = new VarSpace(inter);
            varlist.push(inter_.getProtectVarSpace());
            varlist.push(global);
            global->delReference();
            path = "";
        }

        inter.pushActivation(this);
    }

    static void ActivationTopProgress(Message *msg, Inter &inter, Activation &activation){
        auto *t = dynamic_cast<TopMessage *>(msg);
        if (t)
            t->topProgress(inter, activation);
    };

    /**
     * 析构Activation
     * 注意: 不会自动从inter中弹出
     * 释放Varlist并且将DownMessage压入上层
     */
    Activation::~Activation(){
        if (inter.activation != nullptr)
            down.joinMsg(inter.activation->down);
        else
            down.forEach(ActivationTopProgress, std::ref(inter), std::ref(*this));
    }

    void Activation::endRun() {

    }

    /**
     * 运行代码
     * @param code
     */
    void Activation::runCode(const Code::ByteCode *code){
        auto code_type = code->getType();
        if (code_type == Code::ByteCode::code_start) {  // start 不处理 msg
            auto *none = new Object("None", inter);
            down.pushMessage("NORMAL", new NormalMessage(none));
            none->delReference();
        } else {
            if (code_type == Code::ByteCode::code_element) {
                runCodeElement(code);
            } else
                switch (code->getBlockType()) {
                    case Code::ByteCode::block_p:  // 顺序执行
                        runCodeBlockP(code);
                        break;
                    case Code::ByteCode::block_b:
                        runCodeBlockB(code);
                        break;
                    case Code::ByteCode::block_c:
                        runCodeBlockC(code);
                        break;
                    default:
                        errorLog(aFunCoreLogger, "Error block type.");
                        break;
                }
        }
    }

    void Activation::runCodeElement(const Code::ByteCode *code){
        std::string literaler_name;
        bool in_protect = false;
        Object *obj = nullptr;
        if (inter.checkLiteral(code->getElement(), literaler_name, in_protect)) {
            if (in_protect)
                obj = inter.getProtectVarSpace()->findObject(literaler_name);
            else
                obj = varlist.findObject(literaler_name);
            auto literaler = dynamic_cast<Literaler *>(obj);
            if (literaler != nullptr)
                literaler->getObject(code->getElement(), code->getPrefix(), inter, *this);
            else
                down.pushMessage("ERROR", new ErrorMessage("TypeError", "Error type of literal.", this));
        } else {
            obj = varlist.findObject(code->getElement());
            if (obj != nullptr) {
                auto cbv = dynamic_cast<CallBackVar *>(obj);
                if (cbv != nullptr && cbv->isCallBack(inter, *this))
                    cbv->callBack(inter, *this);
                else
                    down.pushMessage("NORMAL", new NormalMessage(obj));
            } else
                down.pushMessage("ERROR",
                        new ErrorMessage("NameError", std::string("Variable ") + code->getElement() + " not fount.",
                                         this));
        }
    }

    void Activation::runCodeBlockP(const Code::ByteCode *code){
        new ExeActivation(code->getSon(), inter);
    }

    void Activation::runCodeBlockC(const Code::ByteCode *code){
        new FuncActivation(code, inter);
    }

    void Activation::runCodeBlockB(const Code::ByteCode *code){
        new FuncActivation(code, inter);
    }

    Activation::ActivationStatus ExeActivation::getCode(const Code::ByteCode *&code){
        code = next;
        if (code == nullptr)
            return as_end;

        if (!first) {
            auto msg = down.getMessage<NormalMessage>("NORMAL");
            if (msg == nullptr)
                return as_end;
            else
                down.popMessage("NORMAL");
            delete msg;
        }

        first = false;
        line = code->getFileLine();
        path = code->getFilePath();
        next = code->toNext();
        return as_run;
    }

    Activation::VarList::~VarList() {
        for (auto &t : varspace)
            t->delReference();
    }

    /**
     * 访问变量
     * @param name 变量名
     * @return
     */
    Var *Activation::VarList::findVar(const std::string &name){
        Var *ret = nullptr;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == nullptr; tmp++)
            ret = (*tmp)->findVar(name);
        return ret;
    }

    /**
     * 定义变量
     * 若定义出现redefine则退出报错
     * 若出现fail则跳到下一个变量空间尝试定义
     * @param name 变量名
     * @param data 变量（Object）
     * @return
     */
    bool Activation::VarList::defineVar(const std::string &name, Object *data){
        VarSpace::VarOperationFlat ret = VarSpace::vof_fail;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_fail; tmp++)
            ret = (*tmp)->defineVar(name, data);
        return ret == VarSpace::vof_success;
    }

    /**
     * 定义变量
     * 若定义出现redefine则退出报错
     * 若出现fail则跳到下一个变量空间尝试定义
     * @param name 变量名
     * @param data 变量（Var）
     * @return
     */
    bool Activation::VarList::defineVar(const std::string &name, Var *data){
        VarSpace::VarOperationFlat ret = VarSpace::vof_fail;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_fail; tmp++)
            ret = (*tmp)->defineVar(name, data);
        return ret == VarSpace::vof_success;
    }

    /**
     * 设置变量的值
     * 若not_var则跳到下一个变量空间
     * 若fail则结束
     * @param name 变量名
     * @param data 数据
     * @return
     */
    bool Activation::VarList::setVar(const std::string &name, Object *data){
        VarSpace::VarOperationFlat ret = VarSpace::vof_not_var;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_not_var; tmp++)
            ret = (*tmp)->setVar(name, data);
        return ret == VarSpace::vof_success;
    }

    /**
     * 删除变量
     * 若not_var则跳到下一个变量空间
     * 若fail则结束
     * @param name
     * @return
     */
    bool Activation::VarList::delVar(const std::string &name){
        VarSpace::VarOperationFlat ret = VarSpace::vof_not_var;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_not_var; tmp++)
            ret = (*tmp)->delVar(name);
        return ret == VarSpace::vof_success;
    }

    void Activation::VarList::clear(){
        for (auto &t: varspace)
            t->delReference();
        varspace.clear();
    }

    void Activation::VarList::connect(VarList &new_varlist){
        for (auto &t: new_varlist.varspace) {
            t->addReference();
            this->varspace.push_back(t);
        }
    }

    TopActivation::TopActivation(const Code &code, Inter &inter_) : ExeActivation(code, inter_), base{code} {

    }

    FuncActivation::FuncActivation(Function *func_, Inter &inter_)
        : Activation(inter_), status{func_get_func}, on_tail{false}, call{nullptr}, func{func_}, acl_begin{}, acl_end{}  {
        func->addReference();
    }

    FuncActivation::~FuncActivation(){
        if (func != nullptr)
            func->delReference();
        delete call_func;
    }

    Activation::ActivationStatus FuncActivation::getCode(const Code::ByteCode *&code) {
        if (on_tail)
            return as_end;

        if (status == func_first) {
            switch (call->getBlockType()) {
                case Code::ByteCode::block_c:
                    status = func_get_func;
                    code = call->getSon();
                    if (code == nullptr) {
                        line = 0;
                        down.pushMessage("ERROR", new ErrorMessage("SyntaxError", "Callback without code.", this));
                        return as_end;
                    }
                    line = code->getFileLine();
                    if (!code->getFilePath().empty())
                        path = code->getFilePath();
                    return as_run;
                case Code::ByteCode::block_b: {
                    std::string prefix;
                    if (!inter.getEnvVarSpace().findString("sys:prefix", prefix) ||
                        prefix.size() != Inter::PREFIX_COUNT)
                        prefix = "''";
                    char quote = prefix[Inter::prefix_quote];
                    for (Code::ByteCode *var = call->getSon(); var != nullptr; var = var->toNext()) {
                        if (var->getType() != Code::ByteCode::code_element || var->getPrefix() == quote ||
                            inter.checkLiteral(var->getElement()))
                            continue;
                        Object *obj = varlist.findObject(var->getElement());
                        if (obj == nullptr || !dynamic_cast<Function *>(obj) ||
                            !dynamic_cast<Function *>(obj)->isInfix())
                            continue;
                        func = dynamic_cast<Function *>(obj);
                        if (func == nullptr || !func->isInfix())
                            continue;
                        func->addReference();
                        status = func_get_func;
                        break;  /* 跳转到: 执行变量获取前的准备 */
                    }
                    if (status != func_get_func) {
                        line = 0;
                        down.pushMessage("ERROR", new ErrorMessage("SyntaxError", "Callback without code.", this));
                        return as_end;
                    }
                    break;
                }
                default:
                    errorLog(aFunCoreLogger, "Error FuncActivation block type");
                    line = 0;
                    down.pushMessage("ERROR", new ErrorMessage("RuntimeError", "Error FuncActivation block type.", this));
                    return as_end;
            }
        }

        if (status == func_get_func) {
            if (func == nullptr) {
                auto *msg = down.getMessage<NormalMessage>("NORMAL");
                if (msg == nullptr)
                    return as_end;
                else
                    down.popMessage("NORMAL");
                func = dynamic_cast<Function *>(msg->getObject());
                delete msg;
                if (func == nullptr) {
                    down.pushMessage("ERROR", new ErrorMessage("TypeError", "Callback without function.", this));
                    return as_end;
                }
                func->addReference();
            }

            /* Label: 执行变量获取前的准备 */
            status = func_get_arg;
            try {
                call_func = func->getCallFunction(call, inter);
                acl = call_func->getArgCodeList(inter, *this, call);
            } catch (RuntimeError &e) {
                down.pushMessage("ERROR", new ErrorMessage(e.getType(), e.getMessage(), this));
                return as_end;
            }
            acl_begin = acl->begin();
            acl_end = acl->end();
            if (acl_begin != acl_end) {  // 如果有参数需要计算
                code = acl_begin->code;
                line = code->getFileLine();
                path = code->getFilePath();
                return as_run;
            }
        }

        if (acl_begin != acl_end) {  // 获取参数计算结果
            auto *msg = down.getMessage<NormalMessage>("NORMAL");
            if (msg == nullptr)
                return as_end;
            down.popMessage("NORMAL");

            acl_begin->setObject(msg->getObject());
            delete msg;

            acl_begin++;
            if (acl_begin != acl_end) {
                code = acl_begin->code;
                line = code->getFileLine();
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
}