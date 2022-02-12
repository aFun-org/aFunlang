#include "init.h"

namespace aFunit {
    aFuntool::Logger *aFunLogger = nullptr;

    bool aFunInit(InitInfo *info) {
        if (info == nullptr)
            return false;

        aFuntool::getEndian();
        if (setlocale(LC_ALL, "") == nullptr)
            return false;

        setAFunLogger(&info->afun_logger);
        aFuncore::setCoreLogger(&info->core_logger);
        aFunparser::setParserLogger(&info->parser_logger);
        aFuncode::setCodeLogger(&info->code_logger);
        aFuntool::setSysLogger(&info->sys_logger);

        infoLog(aFunLogger, "aFun init success.");
        return true;
    }

}