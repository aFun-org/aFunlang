#include "rt_exe_activation.h"
#include "rt_exception.h"
#include "rt_normal_message.h"
#include "rt_error_message.h"
#include "rt_logger.h"

namespace aFunrt {
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
}