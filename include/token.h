#ifndef AFUN_TOKEN_H
#define AFUN_TOKEN_H
#include "macro.h"

enum af_TokenType {
    TK_ERROR = -1,
    TK_PREFIX = 0,  // 前缀
    TK_LP = 1,
    TK_LB = 2,
    TK_LC = 3,
    TK_RP = 4,
    TK_RB = 5,
    TK_RC = 6,
    TK_ELEMENT_SHORT = 7,
    TK_ELEMENT_LONG = 8,
    TK_COMMENT = 9,
    TK_SPACE = 10,
    TK_EOF = 11,
};

typedef enum af_TokenType af_TokenType;

#endif //AFUN_TOKEN_H
