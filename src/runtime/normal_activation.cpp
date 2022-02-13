#include "rt_normal_activation.h"
#include "rt_exe_activation.h"
#include "rt_func_activation.h"
#include "rt_exception.h"
#include "rt_normal_message.h"
#include "rt_error_message.h"
#include "rt_logger.h"
#include "rt_var_object.h"
#include "rt_varspace_object.h"
#include "rt_func_object.h"
#include "rt_literaler_object.h"
#include "rt_callback_var_object.h"

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
}