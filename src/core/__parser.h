#ifndef AFUN__PARSER_H
#define AFUN__PARSER_H
#include <stdio.h>
#include "__reader.h"
#include "token.h"
#include "parser.h"

enum af_LexicalStatus {
    lex_begin = 0,  // 起始类型
    lex_prefix_block_p = 1,  // 前缀括号 !
    lex_prefix_block_b = 2,  // 前缀括号 @
    lex_prefix_block_c = 3,  // 前缀括号 #
    lex_comment_before = 4,  // 注释
    lex_element_long = 5,
    lex_mutli_comment = 6,  // 多行注释
    lex_uni_comment = 7,  // 当行注释
    lex_mutli_comment_end_before = 8,  // 多行注释遇到;

    lex_prefix = -1,  // prefix类型
    lex_lp = -2,
    lex_lb = -3,
    lex_lc = -4,
    lex_rp = -5,
    lex_rb = -6,
    lex_rc = -7,
    lex_space = -8,
    lex_uni_comment_end = -9,
    lex_mutli_comment_end = -10,
    lex_nul = -11,
    lex_element_short = -12,
    lex_element_long_end = -13,
};

typedef enum af_LexicalStatus af_LexicalStatus;
typedef struct af_Lexical af_Lexical;
typedef struct af_Syntactic af_Syntactic;

struct af_Parser {
    struct af_Reader *reader;
    struct af_Lexical *lexical;
    struct af_Syntactic *syntactic;

    FILE *error;
    bool is_error;  // Parser遇到错误
};

struct af_Lexical {  // 词法匹配器的状态机
    enum af_LexicalStatus status;
    size_t last;  // 最后一次词法匹配的有效长度
    enum af_TokenType token;  // token类型\

    size_t mutli_comment;  // 多行注释嵌套等级
    bool is_end;
    bool is_error;
};

struct af_Syntactic {
    bool back;
    enum af_TokenType token;
    char *text;
};

#endif //AFUN__PARSER_H
