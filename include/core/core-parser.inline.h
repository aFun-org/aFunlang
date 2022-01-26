#ifndef AFUN_CORE_PARSER_INLINE_H
#define AFUN_CORE_PARSER_INLINE_H
#include "core-parser.h"

namespace aFuncore {
    Parser::Parser(Reader &reader_) : reader{reader_} {
        lexical.status = lex_begin;
        lexical.token = TK_PREFIX;
        lexical.last = 0;
        lexical.mutli_comment = 0;
        lexical.is_end = false;
        lexical.is_error = false;

        syntactic.back = false;
        syntactic.token = TK_PREFIX;
        syntactic.text = "";
        syntactic.is_error = false;

        reader.readFirstWord();
    }
}

#endif //AFUN_CORE_PARSER_INLINE_H
