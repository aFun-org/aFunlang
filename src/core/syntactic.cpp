#include "core-parser.h"
#include "init.h"

namespace aFuncore {
    bool Parser::getToken() {
        if (syntactic.back) {
            syntactic.back = false;
            return true;
        }

        syntactic.token = getTokenFromLexical(syntactic.text);
        return syntactic.token != TK_ERROR;  // 非错误则返回true, 遇到错误则返回false
    }

    bool Parser::goBackToken() {
        if (syntactic.back)
            return false;  // 已经有一个回退
        syntactic.back = true;
        return true;
    }

    Code::ByteCode *Parser::codeSelf(Code &code, size_t depth, char prefix) {  // NOLINT
        depth++;
        getToken();
        switch (syntactic.token) {
            case TK_ELEMENT_SHORT:
            case TK_ELEMENT_LONG:
                return  new Code::ByteCode(code, syntactic.text, reader.getFileLine(), prefix);
            case TK_LP: {
                Code::ByteCode *code_list;
                if (depth <= SYNTACTIC_MAX_DEPTH)
                    code_list = codeList(code, depth);
                else {
                    // TODO-szh 错误提示
                    return nullptr;
                }

                getToken();
                switch (syntactic.token) {
                    case TK_RP:
                        break;
                    case TK_ERROR:
                        return nullptr;
                    default:
                        goBackToken();
                        // TODO-szh 错误提示
                        return nullptr;
                }

                return new Code::ByteCode(code, Code::ByteCode::block_p, code_list, reader.getFileLine(), prefix);
            }
            case TK_LB: {
                Code::ByteCode *code_list;
                if (depth <= SYNTACTIC_MAX_DEPTH)
                    code_list = codeList(code, depth);
                else {
                    // TODO-szh 错误提示
                    return nullptr;
                }

                getToken();
                switch (syntactic.token) {
                    case TK_RB:
                        break;
                    case TK_ERROR:
                        return nullptr;
                    default:
                        goBackToken();
                        // TODO-szh 错误提示
                        return nullptr;
                }

                return new Code::ByteCode(code, Code::ByteCode::block_b, code_list, reader.getFileLine(), prefix);
            }
            case TK_LC: {
                Code::ByteCode *code_list;
                if (depth <= SYNTACTIC_MAX_DEPTH)
                    code_list = codeList(code, depth);
                else {
                    // TODO-szh 错误提示
                    return nullptr;
                }

                getToken();
                switch (syntactic.token) {
                    case TK_RC:
                        break;
                    case TK_ERROR:
                        return nullptr;
                    default:
                        goBackToken();
                        // TODO-szh 错误提示
                        return nullptr;
                }

                return new Code::ByteCode(code, Code::ByteCode::block_c, code_list, reader.getFileLine(), prefix);
            }
            case TK_ERROR:
                return nullptr;
            default:
                // TODO-szh 错误提示
                return nullptr;
        }
    }

    Code::ByteCode *Parser::codePrefix(Code &code, size_t depth) {  // NOLINT
        char ch = aFuntool::NUL;
        getToken();
        if (syntactic.token != TK_PREFIX) {
            goBackToken();
            // TODO-szh 错误提示
        } else if (syntactic.text.size() != 1)
            NULL;  // TODO-szh 错误提示
        else
            ch = syntactic.text[0];
        return codeSelf(code, depth, ch);
    }

    Code::ByteCode *Parser::codeList(Code &code, size_t depth) {  // NOLINT
        Code::ByteCode *re = nullptr;
        Code::ByteCode *new_re = nullptr;
        Code::ByteCode *code_list;

        while (true) {
            getToken();
            switch (syntactic.token) {
                case TK_PREFIX:
                    goBackToken();
                    code_list = codePrefix(code, depth);
                    if (code_list != nullptr) {
                        if (new_re == nullptr) {
                            re = code_list;
                            new_re = re->connect(nullptr);
                        } else
                            new_re = new_re->connect(code_list);
                    }
                    break;
                case TK_ELEMENT_SHORT:
                case TK_ELEMENT_LONG:
                case TK_LP:
                case TK_LB:
                case TK_LC:
                    goBackToken();
                    code_list = codeSelf(code, depth, aFuntool::NUL);
                    if (code_list != nullptr) {
                        if (new_re == nullptr) {
                            re = code_list;
                            new_re = re->connect(nullptr);
                        } else
                            new_re = new_re->connect(code_list);
                    }
                    break;
                case TK_ERROR:
                    return re;
                default: /* 结束 */
                    goBackToken();
                    return re;
            }
        }
    }

    Code::ByteCode *Parser::codeListEnd(Code &code) {
        getToken();
        switch (syntactic.token) {
            case TK_EOF:
            case TK_ERROR:
                return nullptr;  // 结束
            case TK_PREFIX:
            case TK_ELEMENT_SHORT:
            case TK_ELEMENT_LONG:
            case TK_LP:
            case TK_LB:
            case TK_LC: {
                goBackToken();
                Code::ByteCode *re = codeList(code, 0);
                getToken();
                if (syntactic.token != TK_EOF && syntactic.token != TK_ERROR)
                    NULL; // TODO-szh 错误提示
                return re;
            }
            default:
                // TODO-szh 错误提示
                return nullptr;
        }
    }

    bool Parser::parserCode(Code &code) {
        Code::ByteCode *bytecode = codeListEnd(code);
        code.getByteCode()->connect(bytecode);
        
        if (syntactic.is_error || reader.isError() || lexical.is_error)
            return false;
        return true;
    }
}