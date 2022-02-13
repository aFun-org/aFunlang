#include "rt_activation.h"
#include "rt_exception.h"
#include "rt_message.h"
#include "rt_logger.h"

namespace aFunrt {
    /**
     * 创建基本Activation
     * 自动继承上层VarList和UpMessage
     * 自动压入inter
     * @param inter_
     */
    NormalActivation::NormalActivation(aFuncore::Inter &inter_)
            : Activation(), inter{inter_}, up{inter_.getActivation() == nullptr ? nullptr : &inter_.getActivation()->getUpStream()}, down{}, line{0} {
        auto activation = dynamic_cast<NormalActivation *>(inter_.getActivation());
        if (activation != nullptr) {
            varlist.connect(activation->getVarlist());
            line = inter_.getActivation()->getFileLine();
            path = inter_.getActivation()->getFilePath();
        } else {
            auto global = new VarSpace(inter);
            varlist.push(global);
            global->delReference();
            path = "";
        }
    }

    static void ActivationTopProgress(aFuncore::Message *msg, aFuncore::Inter &inter, NormalActivation &activation){
        auto *t = dynamic_cast<TopMessage *>(msg);
        if (t)
            t->topProgress(inter, activation);
    };

    /**
     * 析构Activation
     * 注意: 不会自动从inter中弹出
     * 释放Varlist并且将DownMessage压入上层
     */
    NormalActivation::~NormalActivation(){
        if (inter.getActivation() != nullptr)
            down.joinMsg(inter.getActivation()->getDownStream());
        else
            down.forEach(ActivationTopProgress, std::ref(inter), std::ref(*this));
    }

    void NormalActivation::endRun() {

    }

    /**
     * 运行代码
     * @param code
     */
    void NormalActivation::runCode(const aFuncode::Code::ByteCode *code){
        auto code_type = code->getType();
        if (code_type == aFuncode::Code::ByteCode::code_start) {  // start 不处理 msg
            auto *none = new aFuncore::Object("None", inter);
            down.pushMessage("NORMAL", new NormalMessage(none));
            none->delReference();
        } else {
            if (code_type == aFuncode::Code::ByteCode::code_element) {
                runCodeElement(code);
            } else
                switch (code->getBlockType()) {
                    case aFuncode::Code::ByteCode::block_p:  // 顺序执行
                        runCodeBlockP(code);
                        break;
                    case aFuncode::Code::ByteCode::block_b:
                        runCodeBlockB(code);
                        break;
                    case aFuncode::Code::ByteCode::block_c:
                        runCodeBlockC(code);
                        break;
                    default:
                        errorLog(aFunRuntimeLogger, "Error block type.");
                        break;
                }
        }
    }

    void NormalActivation::runCodeElement(const aFuncode::Code::ByteCode *code){
        std::string literaler_name;
        bool in_protect = false;
        aFuncore::Object *obj = nullptr;
        if (inter.checkLiteral(code->getElement(), literaler_name, in_protect)) {
            if (in_protect)
                inter.getEnvVarSpace().findObject(literaler_name, obj);
            else {
                obj = varlist.findObject(literaler_name);
                if (obj == nullptr)
                    inter.getEnvVarSpace().findObject(literaler_name, obj);
            }
            auto literaler = dynamic_cast<Literaler *>(obj);
            if (literaler != nullptr)
                literaler->getObject(code->getElement(), code->getPrefix(), inter, *this);
            else
                down.pushMessage("ERROR", new ErrorMessage("TypeError", "Error type of literal.", inter));
        } else {
            obj = varlist.findObject(code->getElement());
            if (obj == nullptr)
                inter.getEnvVarSpace().findObject(code->getElement(), obj);

            if (obj != nullptr) {
                auto cbv = dynamic_cast<CallBackVar *>(obj);
                if (cbv != nullptr && cbv->isCallBack(inter, *this))
                    cbv->callBack(inter, *this);
                else
                    down.pushMessage("NORMAL", new NormalMessage(obj));
            } else
                down.pushMessage("ERROR",
                                 new ErrorMessage("NameError",
                                                  std::string("Variable ") + code->getElement() + " not fount.", inter));
        }
    }

    void NormalActivation::runCodeBlockP(const aFuncode::Code::ByteCode *code){
        inter.pushActivation(new ExeActivation(code->getSon(), inter));
    }

    void NormalActivation::runCodeBlockC(const aFuncode::Code::ByteCode *code){
        inter.pushActivation(new FuncActivation(code, inter));
    }

    void NormalActivation::runCodeBlockB(const aFuncode::Code::ByteCode *code){
        inter.pushActivation(new FuncActivation(code, inter));
    }

    aFuncore::UpMessageStream &NormalActivation::getUpStream() {
        return up;
    }

    aFuncore::DownMessageStream &NormalActivation::getDownStream() {
        return down;
    }

    aFuntool::FileLine NormalActivation::getFileLine() {
        return line;
    }

    const aFuntool::FilePath &NormalActivation::getFilePath() {
        return path;
    }

    NormalActivation::VarList::~VarList() {
        for (auto &t : varspace)
            t->delReference();
    }

    /**
     * 访问变量
     * @param name 变量名
     * @return
     */
    Var *NormalActivation::VarList::findVar(const std::string &name){
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
    bool NormalActivation::VarList::defineVar(const std::string &name, aFuncore::Object *data){
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
    bool NormalActivation::VarList::defineVar(const std::string &name, Var *data){
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
    bool NormalActivation::VarList::setVar(const std::string &name, aFuncore::Object *data){
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
    bool NormalActivation::VarList::delVar(const std::string &name){
        VarSpace::VarOperationFlat ret = VarSpace::vof_not_var;
        for (auto tmp = varspace.begin(), end = varspace.end(); tmp != end && ret == VarSpace::vof_not_var; tmp++)
            ret = (*tmp)->delVar(name);
        return ret == VarSpace::vof_success;
    }

    void NormalActivation::VarList::clear(){
        for (auto &t: varspace)
            t->delReference();
        varspace.clear();
    }

    void NormalActivation::VarList::connect(VarList &new_varlist){
        for (auto &t: new_varlist.varspace) {
            t->addReference();
            this->varspace.push_back(t);
        }
    }

    NormalActivation::ActivationStatus ExeActivation::getCode(const aFuncode::Code::ByteCode *&code){
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

    TopActivation::TopActivation(const aFuncode::Code &code, aFuncore::Inter &inter_) : ExeActivation(code, inter_), base{code} {

    }

    FuncActivation::FuncActivation(Function *func_, aFuncore::Inter &inter_)
            : NormalActivation(inter_), status{func_get_func}, on_tail{false}, call{nullptr}, func{func_}, acl_begin{}, acl_end{}  {
        func->addReference();
    }

    FuncActivation::~FuncActivation(){
        if (func != nullptr)
            func->delReference();
        delete call_func;
    }

    NormalActivation::ActivationStatus FuncActivation::getCode(const aFuncode::Code::ByteCode *&code) {
        if (on_tail)
            return as_end;

        if (status == func_first) {
            switch (call->getBlockType()) {
                case aFuncode::Code::ByteCode::block_c:
                    status = func_get_func;
                    code = call->getSon();
                    if (code == nullptr) {
                        line = 0;
                        down.pushMessage("ERROR", new ErrorMessage("SyntaxError", "Callback without code.",
                                                                   inter));
                        return as_end;
                    }
                    line = code->getFileLine();
                    if (!code->getFilePath().empty())
                        path = code->getFilePath();
                    return as_run;
                case aFuncode::Code::ByteCode::block_b: {
                    std::string prefix;
                    if (!inter.getEnvVarSpace().findString("sys:prefix-quote", prefix) || prefix.size() != 1)
                        prefix = "'";
                    char quote = prefix[0];
                    for (aFuncode::Code::ByteCode *var = call->getSon(); var != nullptr; var = var->toNext()) {
                        if (var->getType() != aFuncode::Code::ByteCode::code_element || var->getPrefix() == quote ||
                            inter.checkLiteral(var->getElement()))
                            continue;
                        aFuncore::Object *obj = varlist.findObject(var->getElement());
                        if (obj == nullptr)
                            inter.getEnvVarSpace().findObject(var->getElement(), obj);
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
                        down.pushMessage("ERROR", new ErrorMessage("SyntaxError", "Callback without code.",
                                                                   inter));
                        return as_end;
                    }
                    break;
                }
                default:
                    errorLog(aFunRuntimeLogger, "Error FuncActivation block type");
                    line = 0;
                    down.pushMessage("ERROR", new ErrorMessage("RuntimeError", "Error FuncActivation block type.",
                                                               inter));
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
                    down.pushMessage("ERROR", new ErrorMessage("TypeError", "Callback without function.", inter));
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
                down.pushMessage("ERROR", new ErrorMessage(e.getType(), e.getMessage(), inter));
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