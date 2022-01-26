/*
 * 文件名: lexical
 * 目标: aFunlang词法分析
 */
#include <cctype>
#include "parser.h"
#include "init.h"
#include "inter.h"

#ifndef isascii
#define isascii(c) (((c) & ~0x7f) == 0)
#endif

#define isignore(ch) (isascii(ch) && (iscntrl(ch) || isspace(ch) || (ch) == ','))  /* 被忽略的符号 */
#define iselement(ch) (!isascii(ch) || isgraph(ch))  /* 可以作为element的符号 */

#define DEL_TOKEN (0)
#define FINISH_TOKEN (-1)
#define CONTINUE_TOKEN (1)
#define ERROR_TOKEN (-2)

#define printLexicalError(info, parser) do { \
    writeErrorLog(aFunCoreLogger, "[Lexical] %s:%d %s", (parser)->reader.file, (parser)->reader.line, (info ## Log)); \
    printf_stderr(0, "[%s] %s:%d : %s\n", HT_aFunGetText(lexical_n, "Lexical"), (parser)->reader.file, \
                  (parser)->reader.line, info ## Console); \
    (parser)->is_error = true; /* 错误标记在Parser而非Lexical中, Lexical的异常表示lexical停止运行 */ \
} while(0)

#define printLexicalWarning(info, parser) do { \
    writeWarningLog(aFunCoreLogger, "[Lexical] %s:%d %s", (parser)->reader.file, (parser)->reader.line, (info ## Log)); \
    printf_stderr(0, "[%s] %s:%d : %s\n", HT_aFunGetText(lexical_n, "Lexical"), (parser)->reader.file, \
                  (parser)->reader.line, info ## Console); \
} while(0)

namespace aFuncore {
    void Parser::setLexicalLast(LexicalStatus status, TokenType token) {
        lexical.status = status;
        lexical.last = reader.countRead();
        lexical.token = token;
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
     *     -> ALL_PREFIX -> [lex_prefix] # return FINISH_TOKEN
     *     -> ! -> (lex_prefix_block_p)
     *     -> @ -> (lex_prefix_block_b)
     *     -> # -> (lex_prefix_block_c)
     *     -> ( -> [lex_lp] # return FINISH_TOKEN
     *     -> [ -> [lex_lb] # return FINISH_TOKEN
     *     -> { -> [lex_lc] # return FINISH_TOKEN
     *     -> ) -> [lex_rp] # return FINISH_TOKEN
     *     -> ] -> [lex_rb] # return FINISH_TOKEN
     *     -> } -> [lex_rc] # return FINISH_TOKEN
     *     -> ; -> (lex_comment_before)
     *     -> isignore(ch) -> [lex_space]
     *     -> | -> (lex_element_long)
     *     -> iselement(ch) -> [lex_element]
     */
    int Parser::doneBegin(char ch) {
        if (ch == aFuntool::NUL) {
            setLexicalLast(lex_nul, TK_EOF);
            return FINISH_TOKEN;
        } else if (strchr(Inter::ALL_PREFIX, ch)) {  /* 属于前缀 */
            setLexicalLast(lex_prefix, TK_PREFIX);
            return FINISH_TOKEN;
        } else if (strchr("!@#", ch)) {
            switch (ch) {
                case '!':
                    lexical.status = lex_prefix_block_p;
                    return 1;
                case '@':
                    lexical.status = lex_prefix_block_b;
                    return 1;
                case '#':
                    lexical.status = lex_prefix_block_c;
                    return 1;
                default:
                    fatalErrorLog(aFunCoreLogger, EXIT_FAILURE, "Switch illegal characters");
                    return ERROR_TOKEN;
            }
        } else if (strchr("([{)]}", ch)) { /* 括号 */
            switch (ch) {
                case '(':
                    setLexicalLast(lex_lp, TK_LP);
                    return FINISH_TOKEN;
                case '[':
                    setLexicalLast(lex_lb, TK_LB);
                    return FINISH_TOKEN;
                case '{':
                    setLexicalLast(lex_lc, TK_LC);
                    return FINISH_TOKEN;
                case ')':
                    setLexicalLast(lex_rp, TK_RP);
                    return FINISH_TOKEN;
                case ']':
                    setLexicalLast(lex_rb, TK_RB);
                    return FINISH_TOKEN;
                case '}':
                    setLexicalLast(lex_rc, TK_RC);
                    return FINISH_TOKEN;
                default:
                    fatalErrorLog(aFunCoreLogger, EXIT_FAILURE, "Switch illegal characters");
                    return ERROR_TOKEN;
            }
        } else if (ch == ';') {
            lexical.status = lex_comment_before;
            return 1;
        } else if (isignore(ch)) {  // 空白符或控制字符被忽略
            setLexicalLast(lex_space, TK_SPACE);
            return 1;
        } else if (ch == '|') {
            lexical.status = lex_element_long;
            return 1;
        } else if (iselement(ch)) {  // 除空格外的可见字符
            setLexicalLast(lex_element_short, TK_ELEMENT_SHORT);
            return 1;
        }
        // TODO-szh 给出警告
        return DEL_TOKEN;
    }

    /*
     * 状态机图:
     * [lex_prefix_block_p] -> ( -> [lex_lp] # return FINISH_TOKEN
     * [lex_prefix_block_b] -> ( -> [lex_lb] # return FINISH_TOKEN
     * [lex_prefix_block_c] -> ( -> [lex_lc] # return FINISH_TOKEN
     * [lex_prefix_block_p] -> ) -> [lex_rp] # return FINISH_TOKEN
     * [lex_prefix_block_b] -> ) -> [lex_rb] # return FINISH_TOKEN
     * [lex_prefix_block_c] -> ) -> [lex_rc] # return FINISH_TOKEN
     */
    int Parser::donePrefixBlock(char ch) {
        if (ch == '(') {
            switch (lexical.status) {
                case lex_prefix_block_p:
                    setLexicalLast(lex_lp, TK_LP);
                    return FINISH_TOKEN;
                case lex_prefix_block_b:
                    setLexicalLast(lex_lb, TK_LB);
                    return FINISH_TOKEN;
                case lex_prefix_block_c:
                    setLexicalLast(lex_lc, TK_LC);
                    return FINISH_TOKEN;
                default:
                    fatalErrorLog(aFunCoreLogger, EXIT_FAILURE, "Switch illegal characters");
                    return ERROR_TOKEN;
            }
        } else if (ch == ')') {
            switch (lexical.status) {
                case lex_prefix_block_p:
                    setLexicalLast(lex_rp, TK_RP);
                    return FINISH_TOKEN;
                case lex_prefix_block_b:
                    setLexicalLast(lex_rb, TK_RB);
                    return FINISH_TOKEN;
                case lex_prefix_block_c:
                    setLexicalLast(lex_rc, TK_RC);
                    return FINISH_TOKEN;
                default:
                    fatalErrorLog(aFunCoreLogger, EXIT_FAILURE, "Switch illegal characters");
                    return ERROR_TOKEN;
            }
        }
        // TODO-szh 给出警告
        return DEL_TOKEN;
    }

    /*
     * 状态机图:
     * [lex_comment_before]
     *      -> '\n' || NUL -> [lex_uni_comment_end] # return FINISH_TOKEN
     *      -> ; -> (lex_mutli_comment) # mutli_comment = 0
     *      -> other -> (lex_uni_comment)
     */
    int Parser::doneCommentBefore(char ch) {
        if (ch == '\n' || ch == aFuntool::NUL) {
            setLexicalLast(lex_uni_comment_end, TK_COMMENT);
            return FINISH_TOKEN;
        } else if (ch == ';') {  // 多行注释
            lexical.status = lex_mutli_comment;
            lexical.mutli_comment = 0;
            return 1;
        }
        lexical.status = lex_uni_comment;
        return 1;
    }

    /*
     * 状态机图:
     * [lex_uni_comment]
     *      -> '\n' || NUL -> [lex_uni_comment_end] # return FINISH_TOKEN
     *      -> other -> (lex_uni_comment)
     */
    int Parser::doneUniComment(char ch) {
        if (ch == '\n' || ch == aFuntool::NUL) {
            setLexicalLast(lex_uni_comment_end, TK_COMMENT);
            return FINISH_TOKEN;
        }
        lexical.status = lex_uni_comment;
        return 1;
    }

    /*
     * 状态机图:
     * [lex_mutli_comment]
     *      -> NUL -> [lex_mutli_comment_end] # return FINISH_TOKEN; [warning]
     *      -> ; -> (lex_mutli_comment_end_before)
     *      -> other -> (lex_mutli_comment)
     */
    int Parser::doneMutliComment(char ch) {
        if (ch == aFuntool::NUL) {
            lexical.status = lex_mutli_comment_end;
            // TODO-szh 给出警告
            return FINISH_TOKEN;
        } else if (ch == ';')
            lexical.status = lex_mutli_comment_end_before;
        else
            lexical.status = lex_mutli_comment;
        return 1;
    }

    /*
     * 状态机图:
     * [lex_mutli_comment_end_before]
     *      -> NUL -> [lex_mutli_comment_end] # return FINISH_TOKEN; [warning]
     *      -> ; -> (lex_mutli_comment) # mutli_comment++;
     *      -> = ->
     *              mutli_comment == 0 -> [lex_mutli_comment_end] # return FINISH_TOKEN
     *              else -> (lex_mutli_comment)# mutli_comment--;
     */
    int Parser::doneMutliCommentBeforeEnd(char ch) {
        if (ch == aFuntool::NUL) {
            setLexicalLast(lex_mutli_comment_end, TK_COMMENT);
            // TODO-szh 给出警告
            return FINISH_TOKEN;
        } else if (ch == ';') {
            /* 嵌套注释 */
            lexical.mutli_comment++;
            lexical.status = lex_mutli_comment;
        } else if (ch == '=') {
            if (lexical.mutli_comment == 0) {
                /* 注释结束 */
                setLexicalLast(lex_mutli_comment_end, TK_COMMENT);
                return FINISH_TOKEN;
            } else {
                /* 嵌套注释 */
                lexical.mutli_comment--;
                lexical.status = lex_mutli_comment;
            }
        }
        lexical.status = lex_mutli_comment;
        return 1;
    }

    /*
     * 状态机图:
     * [lex_element_long]
     *      -> NUL -> error
     *      -> | -> [lex_element_long_end]
     *      -> other -> (lex_element_long)
     */
    int Parser::doneElementLong(char ch) {
        if (ch == '|') {  // 结束符
            setLexicalLast(lex_element_long_end, TK_ELEMENT_LONG);
            return 1;
        } else if (ch == aFuntool::NUL) {
            // TODO-szh 添加警告
            return ERROR_TOKEN;
        }
        lexical.status = lex_element_long;
        return 1;
    }

    /*
     * 状态机图:
     * [lex_element_long]
     *      -> | -> (lex_element_long)
     *      -> other -> [lex_element_long_end] # return FINISH_TOKEN
     */
    int Parser::doneElementLongEnd(char ch) {
        if (ch == '|') {  // ||表示非结束
            lexical.status = lex_element_long;
            return 1;
        }
        lexical.status = lex_element_long_end;
        return FINISH_TOKEN;
    }

    /*
     * 状态机图:
     * [lex_element_short]
     *      -> !strchr("!@#([{}]);,", ch) && iselement(ch) -> (lex_element_short)
     *      -> other -> (lex_element_short) # return FINISH_TOKEN
     */
    int Parser::doneElementShort(char ch) {
        if (!strchr("!@#([{}]);,", ch) && iselement(ch)) {  // 除空格外的可见字符 (不包括NUL)
            setLexicalLast(lex_element_short, TK_ELEMENT_SHORT);
            return 1;
        }
        lexical.status = lex_element_short;
        return FINISH_TOKEN;
    }

    /*
     * 状态机图:
     * [lex_space]
     *      -> ch != NUL && isignore(ch) -> (lex_space)
     *      -> other -> (lex_space) # return FINISH_TOKEN
     */
    int Parser::doneSpace(char ch) {
        if (ch != aFuntool::NUL && isignore(ch)) {
            setLexicalLast(lex_space, TK_SPACE);
            return 1;
        }
        lexical.status = lex_space;
        return FINISH_TOKEN;
    }

    /*
     * 函数名: getTokenFromLexical
     * 目标: 获取Lexical的TokenType以及相关值
     */
    Parser::TokenType Parser::getTokenFromLexical(std::string &text) {
        Parser::TokenType tt;
        int re;
        lexical.status = lex_begin;
        lexical.last = 0;
        text = "";

        if (lexical.is_end)
            return TK_EOF;
        else if (lexical.is_error || reader.isError())  /* lexical和reader出现异常后不再执行 */
            return TK_ERROR;

        while (true) {
            char ch = reader.getChar();
            if (reader.isError())
                return TK_ERROR;

            if (isascii(ch) && iscntrl(ch) && !isspace(ch) && ch != aFuntool::NUL)  // ascii 控制字符
                NULL;  // TODO-szh 给出警告

            switch (lexical.status) {
                case lex_begin:
                    re = doneBegin(ch);
                    break;
                case lex_prefix_block_p:
                case lex_prefix_block_b:
                case lex_prefix_block_c:
                    re = donePrefixBlock(ch);
                    break;
                case lex_comment_before:
                    re = doneCommentBefore(ch);
                    break;
                case lex_element_long:
                    re = doneElementLong(ch);
                    break;
                case lex_mutli_comment:
                    re = doneMutliComment(ch);
                    break;
                case lex_uni_comment:
                    re = doneUniComment(ch);
                    break;
                case lex_mutli_comment_end_before:
                    re = doneMutliCommentBeforeEnd(ch);
                    break;
                case lex_space:
                    re = doneSpace(ch);
                    break;
                case lex_element_short:
                    re = doneElementShort(ch);
                    break;
                case lex_element_long_end:
                    re = doneElementLongEnd(ch);
                    break;
                default:
                    fatalErrorLog(aFunCoreLogger, EXIT_FAILURE, "Switch illegal characters");
                    re = ERROR_TOKEN;
                    break;
            }

            if (re == ERROR_TOKEN) {
                tt = TK_ERROR;
                break;
            } else if (re == DEL_TOKEN) {  // 删除该token, 继续执行
                char *word = reader.readWord(lexical.last);
                free(word);
                lexical.status = lex_begin;
                lexical.last = 0;
                continue;
            } else if (re == FINISH_TOKEN) {
                char *word = reader.readWord(lexical.last);
                if (word == nullptr) {
                    tt = TK_ERROR;
                    break;
                }

                tt = lexical.token;

                if (tt == TK_ELEMENT_SHORT || tt == TK_PREFIX) {
                    text = word;
                    aFuntool::safeFree(word);
                } else if (tt == TK_ELEMENT_LONG) {
                    char *new_str = aFuntool::safeCalloc<char>(strlen(word) - 2 + 1);  // 去除收尾|

                    bool flat = false;
                    char *p = word + 1;
                    size_t count = 0;
                    for (NULL; *p != aFuntool::NUL; p++) {
                        if (*p == '|' && !flat) {  // 跳过第一个 `|`, 如果是末尾|则自然跳过, 若不是则在遇到第二个`|`时写入数据
                            flat = true; /* count不需要递增 */
                            continue;
                        } else if (*p != '|' && flat)  // 遇到错误
                            break;
                        else
                            flat = false;
                        new_str[count] = *p;
                        count++;
                    }

                    text = new_str;
                    free(word);
                    free(new_str);
                } else
                    free(word);

                if (tt == TK_SPACE || tt == TK_COMMENT) {
                    lexical.status = lex_begin;
                    lexical.last = 0;
                    continue;
                } else if (tt == TK_EOF)
                    lexical.is_end = true;

                break;
            }
        }

        return tt;
    }
}