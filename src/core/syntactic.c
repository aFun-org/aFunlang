#include <ctype.h>
#include "aFunCore.h"
#include "__code.h"
#include "__parser.h"
#include "parserl_warning_error.h"

static void printSyntacticError(char *info, af_Parser *parser) {
    if (parser->error == NULL)
        return;
    fprintf(parser->error, "[Syntactic-Error] %s\n", info);
    parser->is_error = true;
}

static void printSyntacticWarning(char *info, af_Parser *parser) {
    if (parser->error == NULL)
        return;
    fprintf(parser->error, "[Syntactic-Warning] %s\n", info);
}

static bool getToken(af_Parser *parser) {
    if (parser->syntactic->back) {
        parser->syntactic->back = false;
        return true;
    }

    parser->syntactic->token = getTokenFromLexical(&parser->syntactic->text, parser);
    return parser->syntactic->token != TK_ERROR;  // 非错误则返回true, 遇到错误则返回false
}

static bool goBackToken(af_Parser *parser) {
    if (parser->syntactic->back)
        return false;  // 已经有一个回退
    parser->syntactic->back = true;
    return true;
}

static af_Code *codeList(size_t deep, af_Parser *parser);

static af_Code *code(size_t deep, char prefix, af_Parser *parser) {
    af_Code *re;
    af_Code *code_list = NULL;
    deep++;

    getToken(parser);
    switch (parser->syntactic->token) {
        case TK_ELEMENT_SHORT:
        case TK_ELEMENT_LONG:
            re = makeElementCode(parser->syntactic->text, prefix, parser->reader->line, NULL);
            free(parser->syntactic->text);
            break;
        case TK_LP:
            if (deep <= SYNTACTIC_MAX_DEEP)
                code_list = codeList(deep, parser);
            else
                printSyntacticError(SYNTACTIC_TOO_DEEP(), parser);

            getToken(parser);
            switch (parser->syntactic->token) {
                case TK_RP:
                    break;
                case TK_ERROR:
                    freeAllCode(code_list);
                    return NULL;
                default:
                    goBackToken(parser);
                    printSyntacticError(CodeEndError(") or !)"), parser);
                    break;
            }

            re = makeBlockCode(parentheses, code_list, prefix, parser->reader->line, NULL, NULL);
            break;
        case TK_LB:
            if (deep <= SYNTACTIC_MAX_DEEP)
                code_list = codeList(deep, parser);
            else
                printSyntacticError(SYNTACTIC_TOO_DEEP(), parser);

            getToken(parser);
            switch (parser->syntactic->token) {
                case TK_RB:
                    break;
                case TK_ERROR:
                    freeAllCode(code_list);
                    return NULL;
                default:
                    goBackToken(parser);
                    printSyntacticError(CodeEndError("] or @)"), parser);
                    break;
            }

            re = makeBlockCode(brackets, code_list, prefix, parser->reader->line, NULL, NULL);
            break;
        case TK_LC:
            if (deep <= SYNTACTIC_MAX_DEEP)
                code_list = codeList(deep, parser);
            else
                printSyntacticError(SYNTACTIC_TOO_DEEP(), parser);

            getToken(parser);
            switch (parser->syntactic->token) {
                case TK_RC:
                    break;
                case TK_ERROR:
                    freeAllCode(code_list);
                    return NULL;
                default:
                    goBackToken(parser);
                    printSyntacticError(CodeEndError("} or #)"), parser);
                    break;
            }

            re = makeBlockCode(curly, code_list, prefix, parser->reader->line, NULL, NULL);
            break;
        case TK_ERROR:
            return NULL;
        default:
            printSyntacticError(CodeStartError(), parser);
            return NULL;
    }

    if (re == NULL)
        printSyntacticError(MakeCodeFail(), parser);
    return re;
}

static af_Code *codePrefix(size_t deep, af_Parser *parser) {
    char ch = NUL;
    getToken(parser);
    if (parser->syntactic->token != TK_PREFIX) {
        goBackToken(parser);
        printSyntacticError(PREFIX_ERROR(codePrefix), parser);
    } else if (STR_LEN( parser->syntactic->text) != 1) {
        printSyntacticError(PREFIX_ERROR(codePrefix), parser);
        free(parser->syntactic->text);
    } else {
        ch = *(parser->syntactic->text);
        free(parser->syntactic->text);
    }

    return code(deep, ch, parser);
}

static af_Code *codeList(size_t deep, af_Parser *parser) {
    af_Code *re = NULL;
    af_Code **pre = &re;
    af_Code *code_list;

    while (1) {
        getToken(parser);
        switch (parser->syntactic->token) {
            case TK_PREFIX:
                goBackToken(parser);
                code_list = codePrefix(deep, parser);
                if (code_list != NULL)
                    pre = &(pushCode(pre, code_list)->next);
                break;

            case TK_ELEMENT_SHORT:
            case TK_ELEMENT_LONG:
            case TK_LP:
            case TK_LB:
            case TK_LC:
                goBackToken(parser);
                code_list = code(deep, NUL, parser);
                if (code_list != NULL)
                    pre = &(pushCode(pre, code_list)->next);
                break;
            case TK_ERROR:
                freeAllCode(re);
                return NULL;
            default: /* 结束 */
                goBackToken(parser);
                return re;
        }
    }
}

static af_Code *codeListEnd(af_Parser *parser) {
    af_Code *re = NULL;
    af_Code **pre = &re;
    af_Code *code_list;

    getToken(parser);
    switch (parser->syntactic->token) {
        case TK_EOF:
            break;  // 结束
        case TK_PREFIX:
        case TK_ELEMENT_SHORT:
        case TK_ELEMENT_LONG:
        case TK_LP:
        case TK_LB:
        case TK_LC:
            goBackToken(parser);
            code_list = codeList(0, parser);
            pushCode(pre, code_list);

            getToken(parser);
            switch (parser->syntactic->token) {
                case TK_EOF:
                    break;  // 正常结束
                case TK_ERROR:
                    freeAllCode(re);
                    return NULL;
                default:
                    printSyntacticError(CodeListEndError(), parser);
                    freeAllCode(re);
                    return NULL;
            }
            break;
        case TK_ERROR:
            return NULL;
        default:
            printSyntacticError(CodeListStartError(), parser);
            return NULL;
    }

    return re;
}

af_Code *parserCode(FilePath file, af_Parser *parser) {
    af_Code *code = codeListEnd(parser);
    if (file == NULL)
        return NULL;

    if (parser->is_error) {
        freeAllCode(code);
        return NULL;
    }

    if (code != NULL)
        code->path = pathCopy(file);
    return code;
}
