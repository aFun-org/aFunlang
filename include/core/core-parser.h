﻿#ifndef AFUN_CORE_PARSER_H
#define AFUN_CORE_PARSER_H
#include "aFunToolExport.h"
#include "reader.h"
#include "code.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Parser {
    public:
        typedef enum LexicalStatus {
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
        } LexicalStatus;

        typedef enum TokenType {
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
        } TokenType;

        inline explicit Parser(Reader &reader_);

        TokenType getTokenFromLexical(std::string &text);
        bool parserCode(Code &code);
    private:
        typedef enum DoneStatus {
            DEL_TOKEN = 0,
            FINISH_TOKEN = -1,
            CONTINUE_TOKEN = 1,
            ERROR_TOKEN = -2
        } DoneStatus;

        Reader &reader;

        struct {
            LexicalStatus status;
            TokenType token;  // token类型
            size_t last;  // 最后一次词法匹配的有效长度
            size_t mutli_comment;  // 多行注释嵌套等级
            bool is_end;
            bool is_error;
        } lexical;

        struct {
            bool back;
            TokenType token;
            std::string text;
            bool is_error;
        } syntactic;

        void setLexicalLast(LexicalStatus status, TokenType token);
        DoneStatus doneBegin(char ch);
        DoneStatus donePrefixBlock(char ch);
        DoneStatus doneCommentBefore(char ch);
        DoneStatus doneUniComment(char ch);
        DoneStatus doneMutliComment(char ch);
        DoneStatus doneMutliCommentBeforeEnd(char ch);
        DoneStatus doneElementLong(char ch);
        DoneStatus doneElementLongEnd(char ch);
        DoneStatus doneElementShort(char ch);
        DoneStatus doneSpace(char ch);

        bool getToken();
        bool goBackToken();

        static const size_t SYNTACTIC_MAX_DEPTH = 218;
        Code::ByteCode *codeSelf(Code &code, size_t deep, char prefix);
        Code::ByteCode *codePrefix(Code &code, size_t deep);
        Code::ByteCode *codeList(Code &code, size_t deep);
        Code::ByteCode *codeListEnd(Code &code);
    };

}

#include "core-parser.inline.h"
#endif //AFUN_CORE_PARSER_H
