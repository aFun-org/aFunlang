#ifndef AFUN_CORE_PARSER_INLINE_H
#define AFUN_CORE_PARSER_INLINE_H
#include "core-parser.h"

namespace aFuncore {
    inline Parser::Parser(Reader &reader_)
        : reader{reader_},
          lexical{lex_begin, TK_PREFIX, 0, 0, false, false},
          syntactic{false, TK_PREFIX, "", false}{

        reader.readFirstWord();
    }

    inline Parser::ParserEvent Parser::popEvent() {
        ParserEvent pop = event.front();
        event.pop_front();
        return pop;
    }

    inline size_t Parser::countEvent() const {
        return event.size();
    }

    inline void Parser::pushEvent(const ParserEvent &new_event) {
        event.push_back(new_event);
    }

    inline void Parser::pushEvent(ParserEvent &&new_event) {
        event.push_back(new_event);
    }

    inline const Parser::ParserEvent &Parser::checkEvent() const {
            return event.front();
    }
}

#endif //AFUN_CORE_PARSER_INLINE_H
