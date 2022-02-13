#ifndef AFUN_PARSER_SYNTAX_INLINE_H
#define AFUN_PARSER_SYNTAX_INLINE_H
#include "parser_syntax.h"

namespace aFunparser {
    Parser::Parser(Reader &reader_)
        : reader{reader_},
          lexical{lex_begin, TK_PREFIX, 0, 0, false, false},
          syntactic{false, TK_PREFIX, "", false}{

        reader.readFirstWord();
    }

    Parser::ParserEvent Parser::popEvent() {
        ParserEvent pop = event.front();
        event.pop();
        return pop;
    }

    size_t Parser::countEvent() const {
        return event.size();
    }

    void Parser::pushEvent(const ParserEvent &new_event) {
        event.push(new_event);
    }

    void Parser::pushEvent(ParserEvent &&new_event) {
        event.push(new_event);
    }

    const Parser::ParserEvent &Parser::checkEvent() const {
            return event.front();
    }
}

#endif //AFUN_PARSER_SYNTAX_INLINE_H
