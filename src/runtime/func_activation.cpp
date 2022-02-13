#include "rt_func_activation.h"
#include "rt_exception.h"
#include "rt_normal_message.h"
#include "rt_error_message.h"
#include "rt_logger.h"

namespace aFunrt {
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