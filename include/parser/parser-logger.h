#ifndef AFUN_PARSER_LOGGER_H
#define AFUN_PARSER_LOGGER_H
#include "aFunParserExport.h"
#include "aFuntool.h"

namespace aFunparser {
    AFUN_PARSER_EXPORT extern aFuntool::Logger *aFunParserLogger;
    AFUN_STATIC void setParserLogger(aFuntool::Logger *log);
}

#include "parser-logger.inline.h"
#endif //AFUN_PARSER_LOGGER_H
