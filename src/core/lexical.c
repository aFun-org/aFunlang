/*
 * 文件名: lexical
 * 目标: aFunlang词法分析
 */
#include <ctype.h>
#include "aFunCore.h"
#include "__parser.h"
#include "parserl_warning_error.h"

#ifndef isascii
#define isascii (((c) & ~0x7f) == 0)
#endif

#define isignore(ch) (isascii(ch) && (iscntrl(ch) || isspace(ch) || ch == ','))  /* 被忽略的符号 */
#define iselement(ch) (!isascii(ch) || isgraph(ch))  /* 可以作为element的符号 */

static void printLexicalError(char *info, af_Parser *parser) {
    writeErrorLog(aFunCoreLogger, "[Lexical] %s", info);
    parser->is_error = true;
}

static void printLexicalWarning(char *info, af_Parser *parser) {
    writeWarningLog(aFunCoreLogger, "[Lexical] %s", info);
}

static void setLexicalLast(af_LexicalStatus status, af_TokenType token, af_Parser *parser) {
    parser->lexical->status = status;
    parser->lexical->last = parser->reader->read - parser->reader->buf;
    parser->lexical->token = token;
}

/*
 * 函数族: done系列 (doneXXX)
 * 目标: 用于把转台xxx转换为其他状态
 * 返回值: 1 正常
 * 返回值: 0 遇到错误, 仍可继续
 * 返回值: -1 正常, 不可继续 -> 必须设置 setLexicalLast
 * 返回值: -2 遇到错误, 不可继续
 * 注意: 函数使用前不在检查`status`是否正确
 */

/*
 * 状态机图:
 * [lex_begin]
 *     -> NUL -> (lex_nul)
 *     -> ALL_PREFIX -> [lex_prefix] # return -1
 *     -> ! -> (lex_prefix_block_p)
 *     -> @ -> (lex_prefix_block_b)
 *     -> # -> (lex_prefix_block_c)
 *     -> ( -> [lex_lp] # return -1
 *     -> [ -> [lex_lb] # return -1
 *     -> { -> [lex_lc] # return -1
 *     -> ) -> [lex_rp] # return -1
 *     -> ] -> [lex_rb] # return -1
 *     -> } -> [lex_rc] # return -1
 *     -> ; -> (lex_comment_before)
 *     -> isignore(ch) -> [lex_space]
 *     -> | -> (lex_element_long)
 *     -> iselement(ch) -> [lex_element]
 */

static int doneBegin(char ch, af_Parser *parser) {
    if (ch == NUL) {
        setLexicalLast(lex_nul, TK_EOF, parser);
        return -1;
    } else if (strchr(ALL_PREFIX, ch)) {  /* 属于前缀 */
        setLexicalLast(lex_prefix, TK_PREFIX, parser);
        return -1;
    } else if (strchr("!@#", ch)) {
        switch (ch) {
            case '!':
                parser->lexical->status = lex_prefix_block_p;
                return 1;
            case '@':
                parser->lexical->status = lex_prefix_block_b;
                return 1;
            case '#':
                parser->lexical->status = lex_prefix_block_c;
                return 1;
            default:
                printLexicalError(SYS_ILLEGAL_CHAR(lex_beging), parser);
                return -2;
        }
    } else if (strchr("([{)]}", ch)) { /* 括号 */
        switch (ch) {
            case '(':
                setLexicalLast(lex_lp, TK_LP, parser);
                return -1;
            case '[':
                setLexicalLast(lex_lb, TK_LB, parser);
                return -1;
            case '{':
                setLexicalLast(lex_lc, TK_LC, parser);
                return -1;
            case ')':
                setLexicalLast(lex_rp, TK_RP, parser);
                return -1;
            case ']':
                setLexicalLast(lex_rb, TK_RB, parser);
                return -1;
            case '}':
                setLexicalLast(lex_rc, TK_RC, parser);
                return -1;
            default:
                printLexicalError(SYS_ILLEGAL_CHAR(lex_beging), parser);
                return -2;
        }
    } else if (ch == ';') {
        parser->lexical->status = lex_comment_before;
        return 1;
    } else if (isignore(ch)) {  // 空白符或控制字符被忽略
        setLexicalLast(lex_space, TK_SPACE, parser);
        return 1;
    } else if (ch == '|') {
        parser->lexical->status = lex_element_long;
        return 1;
    } else if (iselement(ch)) {  // 除空格外的可见字符
        setLexicalLast(lex_element_short, TK_ELEMENT_SHORT, parser);
        return 1;
    }
    printLexicalError(ILLEGAL_CHAR(lex_beging), parser);
    return 0;
}

/*
 * 状态机图:
 * [lex_prefix_block_p] -> ( -> [lex_lp] # return -1
 * [lex_prefix_block_b] -> ( -> [lex_lb] # return -1
 * [lex_prefix_block_c] -> ( -> [lex_lc] # return -1
 * [lex_prefix_block_p] -> ) -> [lex_rp] # return -1
 * [lex_prefix_block_b] -> ) -> [lex_rb] # return -1
 * [lex_prefix_block_c] -> ) -> [lex_rc] # return -1
 */
static int donePrefixBlock(char ch, af_Parser *parser) {
    if (ch == '(') {
        switch (parser->lexical->status) {
            case lex_prefix_block_p:
                setLexicalLast(lex_lp, TK_LP, parser);
                return -1;
            case lex_prefix_block_b:
                setLexicalLast(lex_lb, TK_LB, parser);
                return -1;
            case lex_prefix_block_c:
                setLexicalLast(lex_lc, TK_LC, parser);
                return -1;
            default:
                printLexicalError(SYS_ERROR_STATUS(lex_prefix_block), parser);
                return -2;
        }
    } else if (ch == ')') {
        switch (parser->lexical->status) {
            case lex_prefix_block_p:
                setLexicalLast(lex_rp, TK_RP, parser);
                return -1;
            case lex_prefix_block_b:
                setLexicalLast(lex_rb, TK_RB, parser);
                return -1;
            case lex_prefix_block_c:
                setLexicalLast(lex_rc, TK_RC, parser);
                return -1;
            default:
                printLexicalError(SYS_ERROR_STATUS(lex_prefix_block), parser);
                return -2;
        }
    }
    printLexicalError(ILLEGAL_CHAR(lex_prefix_block), parser);
    return 0;
}

/*
 * 状态机图:
 * [lex_comment_before]
 *      -> '\n' || NUL -> [lex_uni_comment_end] # return -1
 *      -> ; -> (lex_mutli_comment) # mutli_comment = 0
 *      -> other -> (lex_uni_comment)
 */
static int doneCommentBefore(char ch, af_Parser *parser) {
    if (ch == '\n' || ch == NUL) {
        setLexicalLast(lex_uni_comment_end, TK_COMMENT, parser);
        return -1;
    } else if (ch == ';') {  // 多行注释
        parser->lexical->status = lex_mutli_comment;
        parser->lexical->mutli_comment = 0;
        return 1;
    }
    parser->lexical->status = lex_uni_comment;
    return 1;
}

/*
 * 状态机图:
 * [lex_uni_comment]
 *      -> '\n' || NUL -> [lex_uni_comment_end] # return -1
 *      -> other -> (lex_uni_comment)
 */
static int doneUniComment(char ch, af_Parser *parser) {
    if (ch == '\n' || ch == NUL) {
        setLexicalLast(lex_uni_comment_end, TK_COMMENT, parser);
        return -1;
    }
    parser->lexical->status = lex_uni_comment;
    return 1;
}

/*
 * 状态机图:
 * [lex_mutli_comment]
 *      -> NUL -> [lex_mutli_comment_end] # return -1; [warning]
 *      -> ; -> (lex_mutli_comment_end_before)
 *      -> other -> (lex_mutli_comment)
 */
static int doneMutliComment(char ch, af_Parser *parser) {
    if (ch == NUL) {
        parser->lexical->status = lex_mutli_comment_end;
        printLexicalWarning(INCOMPLETE_FILE(lex_mutli_comment), parser);
        return -1;
    } else if (ch == ';')
        parser->lexical->status = lex_mutli_comment_end_before;
    else
        parser->lexical->status = lex_mutli_comment;
    return 1;
}

/*
 * 状态机图:
 * [lex_mutli_comment_end_before]
 *      -> NUL -> [lex_mutli_comment_end] # return -1; [warning]
 *      -> ; -> (lex_mutli_comment) # mutli_comment++;
 *      -> = ->
 *              mutli_comment == 0 -> [lex_mutli_comment_end] # return -1
 *              else -> (lex_mutli_comment)# mutli_comment--;
 */
static int doneMutliCommentBeforeEnd(char ch, af_Parser *parser) {
    if (ch == NUL) {
        printLexicalWarning(INCOMPLETE_FILE(lex_mutli_comment_end_before), parser);
        setLexicalLast(lex_mutli_comment_end, TK_COMMENT, parser);
        return -1;
    } else if (ch == ';') {
        /* 嵌套注释 */
        parser->lexical->mutli_comment++;
        parser->lexical->status = lex_mutli_comment;
    } else if (ch == '=') {
        if (parser->lexical->mutli_comment == 0) {
            /* 注释结束 */
            setLexicalLast(lex_mutli_comment_end, TK_COMMENT, parser);
            return -1;
        } else {
            /* 嵌套注释 */
            parser->lexical->mutli_comment--;
            parser->lexical->status = lex_mutli_comment;
        }
    }
    parser->lexical->status = lex_mutli_comment;
    return 1;
}

/*
 * 状态机图:
 * [lex_element_long]
 *      -> NUL -> error
 *      -> | -> [lex_element_long_end]
 *      -> other -> (lex_element_long)
 */
static int doneElementLong(char ch, af_Parser *parser) {
    if (ch == '|') {  // 结束符
        setLexicalLast(lex_element_long_end, TK_ELEMENT_LONG, parser);
        return 1;
    } else if (ch == NUL) {
        printLexicalError(INCOMPLETE_FILE(lex_element_long), parser);
        return -2;
    }
    parser->lexical->status = lex_element_long;
    return 1;
}

/*
 * 状态机图:
 * [lex_element_long]
 *      -> | -> (lex_element_long)
 *      -> other -> [lex_element_long_end] # return -1
 */
static int doneElementLongEnd(char ch, af_Parser *parser) {
    if (ch == '|') {  // ||表示非结束
        parser->lexical->status = lex_element_long;
        return 1;
    }
    parser->lexical->status = lex_element_long_end;
    return -1;
}

/*
 * 状态机图:
 * [lex_element_short]
 *      -> !strchr("!@#([{}]);,", ch) && iselement(ch) -> (lex_element_short)
 *      -> other -> (lex_element_short) # return -1
 */
static int doneElementShort(char ch, af_Parser *parser) {
    if (!strchr("!@#([{}]);,", ch) && iselement(ch)) {  // 除空格外的可见字符 (不包括NUL)
        setLexicalLast(lex_element_short, TK_ELEMENT_SHORT, parser);
        return 1;
    }
    parser->lexical->status = lex_element_short;
    return -1;
}

/*
 * 状态机图:
 * [lex_space]
 *      -> ch != NUL && isignore(ch) -> (lex_space)
 *      -> other -> (lex_space) # return -1
 */
static int doneSpace(char ch, af_Parser *parser) {
    if (ch != NUL && isignore(ch)) {
        setLexicalLast(lex_space, TK_SPACE, parser);
        return 1;
    }
    parser->lexical->status = lex_space;
    return -1;
}

/*
 * 函数名: getTokenFromLexical
 * 目标: 获取Lexical的TokenType以及相关值
 */
af_TokenType getTokenFromLexical(char **text, af_Parser *parser) {
    af_TokenType tt;
    int re;
    parser->lexical->status = lex_begin;
    parser->lexical->last = 0;

    if (parser->lexical->is_end) {
        *text = NULL;
        return TK_EOF;
    } else if (parser->lexical->is_error) {
        *text = NULL;
        return TK_ERROR;
    }

    while (1) {
        char ch = getChar(parser->reader);
        if (isascii(ch) && iscntrl(ch) && !isspace(ch) && ch != NUL)  // ascii 控制字符
            printLexicalWarning(INCULDE_CONTROL(base), parser);

        switch (parser->lexical->status) {
            case lex_begin:
                re = doneBegin(ch, parser);
                break;
            case lex_prefix_block_p:
            case lex_prefix_block_b:
            case lex_prefix_block_c:
                re = donePrefixBlock(ch, parser);
                break;
            case lex_comment_before:
                re = doneCommentBefore(ch, parser);
                break;
            case lex_element_long:
                re = doneElementLong(ch, parser);
                break;
            case lex_mutli_comment:
                re = doneMutliComment(ch, parser);
                break;
            case lex_uni_comment:
                re = doneUniComment(ch, parser);
                break;
            case lex_mutli_comment_end_before:
                re = doneMutliCommentBeforeEnd(ch, parser);
                break;
            case lex_space:
                re = doneSpace(ch, parser);
                break;
            case lex_element_short:
                re = doneElementShort(ch, parser);
                break;
            case lex_element_long_end:
                re = doneElementLongEnd(ch, parser);
                break;
            default:
                printLexicalError(SYS_ERROR_STATUS(base), parser);
                re = -3;
                break;
        }

        if (re == -1) {
            char *word = readWord(parser->lexical->last, parser->reader);
            tt = parser->lexical->token;

            if (tt == TK_ELEMENT_SHORT || tt == TK_PREFIX)
                *text = word;
            else if (tt == TK_ELEMENT_LONG) {
                char *new = NEW_STR(STR_LEN(word) - 2);  // 去除收尾|

                bool flat = false;
                char *p = word + 1;
                size_t count = 0;
                for(NULL; *p != NUL; p++) {
                    if (*p == '|' && !flat) {  // 跳过第一个 `|`, 如果是末尾|则自然跳过, 若不是则在遇到第二个`|`时写入数据
                        flat = true; /* count不需要递增 */
                        continue;
                    } else if (*p != '|' && flat)  // 遇到错误
                        break;
                    else
                        flat = false;
                    new[count] = *p;
                    count++;
                }

                *text = strCopy(new);
                free(word);
                free(new);
            } else
                free(word);

            if (tt == TK_SPACE || tt == TK_COMMENT) {
                parser->lexical->status = lex_begin;
                parser->lexical->last = 0;
                continue;
            } else if (tt == TK_EOF)
                parser->lexical->is_end = true;

            break;
        } else if (re == 0) {  // 删除该token, 继续执行
            char *word = readWord(parser->lexical->last, parser->reader);
            free(word);
            parser->lexical->status = lex_begin;
            parser->lexical->last = 0;
            continue;
        } else if (re == -2 || re == -3) {
            tt = TK_ERROR;
            *text = NULL;
            parser->lexical->is_error = true;
            break;
        }
    }

    return tt;
}
