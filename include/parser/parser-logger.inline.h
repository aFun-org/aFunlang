#ifndef AFUN_PARSER_LOGGER_INLINE_H
#define AFUN_PARSER_LOGGER_INLINE_H
#include "parser-logger.h"

namespace aFunparser {
    void setParserLogger(aFuntool::Logger *log) {
        aFunParserLogger = log;
    }
}

#endif //AFUN_PARSER_LOGGER_INLINE_H
