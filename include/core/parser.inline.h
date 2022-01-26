﻿#ifndef AFUN_PARSER_INLINE_H
#define AFUN_PARSER_INLINE_H
#include "parser.h"

namespace aFuncore {
    Parser::Parser(Reader &reader_) : reader{reader_} {
        lexical.status = lex_begin;
        lexical.token = TK_PREFIX;
        lexical.last = 0;
        lexical.mutli_comment = 0;
        lexical.is_end = false;
        lexical.is_error = false;

        reader.readFirstWord();
    }
}

#endif //AFUN_PARSER_INLINE_H
