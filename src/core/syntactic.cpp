#include "core-parser.h"
#include "core-logger.h"

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

    aFuncode::Code::ByteCode *Parser::codeSelf(aFuncode::Code &code, size_t depth, char prefix) {  // NOLINT
        depth++;
        getToken();
        switch (syntactic.token) {
            case TK_ELEMENT_SHORT:
            case TK_ELEMENT_LONG:
                return new aFuncode::Code::ByteCode(code, syntactic.text, reader.getFileLine(), prefix);
            case TK_LP: {
                aFuncode::Code::ByteCode *code_list;
                if (depth <= SYNTACTIC_MAX_DEPTH) {
                    code_list = codeList(code, depth);
                    code_list = new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_p, code_list, reader.getFileLine(), prefix);
                } else {
                    pushEvent({ParserEvent::syntactic_error_nested_too_deep, reader.getFileLine(), ""});
                    syntactic.is_error = true;
                    return nullptr;
                }

                getToken();
                if (syntactic.token != TK_RP && syntactic.token != TK_ERROR) {
                    goBackToken();
                    pushEvent({ParserEvent::syntactic_error_block_p_end, reader.getFileLine(), ""});
                    syntactic.is_error = true;
                }

                return code_list;
            }
            case TK_LB: {
                aFuncode::Code::ByteCode *code_list;
                if (depth <= SYNTACTIC_MAX_DEPTH) {
                    code_list = codeList(code, depth);
                    code_list = new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_b, code_list, reader.getFileLine(), prefix);
                } else {
                    pushEvent({ParserEvent::syntactic_error_nested_too_deep, reader.getFileLine(), ""});
                    syntactic.is_error = true;
                    return nullptr;
                }

                getToken();
                if (syntactic.token != TK_RB && syntactic.token != TK_ERROR) {
                    goBackToken();
                    pushEvent({ParserEvent::syntactic_error_block_b_end, reader.getFileLine(), ""});
                    syntactic.is_error = true;
                }

                return code_list;
            }
            case TK_LC: {
                aFuncode::Code::ByteCode *code_list;
                if (depth <= SYNTACTIC_MAX_DEPTH) {
                    code_list = codeList(code, depth);
                    code_list = new aFuncode::Code::ByteCode(code, aFuncode::Code::ByteCode::block_c, code_list, reader.getFileLine(), prefix);
                } else {
                    pushEvent({ParserEvent::syntactic_error_nested_too_deep, reader.getFileLine(), ""});
                    syntactic.is_error = true;
                    return nullptr;
                }

                getToken();
                if (syntactic.token != TK_RC && syntactic.token != TK_ERROR) {
                    goBackToken();
                    pushEvent({ParserEvent::syntactic_error_block_c_end, reader.getFileLine(), ""});
                    syntactic.is_error = true;
                }

                return code_list;
            }
            case TK_ERROR:
                return nullptr;
            default:
                pushEvent({ParserEvent::parser_error_unknown, reader.getFileLine(), ""});
                syntactic.is_error = true;
                return nullptr;
        }
    }

    aFuncode::Code::ByteCode *Parser::codePrefix(aFuncode::Code &code, size_t depth) {  // NOLINT
        char ch = aFuntool::NUL;
        getToken();
        if (syntactic.token != TK_PREFIX) {
            goBackToken();
            pushEvent({ParserEvent::syntactic_error_prefix, reader.getFileLine(), ""});
            syntactic.is_error = true;
        } else if (syntactic.text.size() != 1) {
            pushEvent({ParserEvent::syntactic_error_prefix, reader.getFileLine(), ""});
            syntactic.is_error = true;
        } else
            ch = syntactic.text[0];
        return codeSelf(code, depth, ch);
    }

    aFuncode::Code::ByteCode *Parser::codeList(aFuncode::Code &code, size_t depth) {  // NOLINT
        aFuncode::Code::ByteCode *re = nullptr;
        aFuncode::Code::ByteCode *new_re = nullptr;
        aFuncode::Code::ByteCode *code_list;

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

    aFuncode::Code::ByteCode *Parser::codeListEnd(aFuncode::Code &code) {
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
                aFuncode::Code::ByteCode *re = codeList(code, 0);
                getToken();
                if (syntactic.token != TK_EOF && syntactic.token != TK_ERROR) {
                    pushEvent({ParserEvent::parser_error_unknown, reader.getFileLine(), ""});
                    syntactic.is_error = true;
                }
                return re;
            }
            default:
                pushEvent({ParserEvent::parser_error_unknown, reader.getFileLine(), ""});
                syntactic.is_error = true;
                return nullptr;
        }
    }

    bool Parser::parserCode(aFuncode::Code &code) {
        aFuncode::Code::ByteCode *bytecode = codeListEnd(code);
        code.getByteCode()->connect(bytecode);
        
        if (syntactic.is_error || reader.isError() || lexical.is_error)
            return false;
        return true;
    }
}