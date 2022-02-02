#include "__main.h"

/*
 * 函数名: printError
 * 目标: 打印参数错误信息
 */
void printError(ff_FFlags *ff) {
    errorLog(aFun::aFunLogger, "%s argument error", ff_getChild(ff));
    aFun::cout << "Command line argument error (" << ff_getChild(ff) << ")\n";
    printHelp();
}

void printParserError(aFun::Parser &parser, const aFun::FilePath &file) {
    while (parser.countEvent() != 0) {
        auto event = parser.popEvent();
        switch (event.type) {
            case aFun::Parser::ParserEvent::lexical_error_char:
                aFun::cout << "ERROR: Parser find an error character.";
                break;
            case aFun::Parser::ParserEvent::lexical_error_element_end:
                aFun::cout << "ERROR: Long element without end.";
                break;
            case aFun::Parser::ParserEvent::syntactic_error_nested_too_deep:
                aFun::cout << "ERROR: Nested too deep.";
                break;
            case aFun::Parser::ParserEvent::syntactic_error_block_p_end:
                aFun::cout << "ERROR: '(' without end.";
                break;
            case aFun::Parser::ParserEvent::syntactic_error_block_b_end:
                aFun::cout << "ERROR: '[' without end.";
                break;
            case aFun::Parser::ParserEvent::syntactic_error_block_c_end:
                aFun::cout << "ERROR: '{' without end.";
                break;
            case aFun::Parser::ParserEvent::syntactic_error_prefix:
                aFun::cout << "ERROR: Parser find an error prefix character.";
                break;
            case aFun::Parser::ParserEvent::reader_error:
                aFun::cout << "ERROR: Reader error.";
                break;
            case aFun::Parser::ParserEvent::lexical_warning_comment_end:
                aFun::cout << "WARNING: Comment without end.";
                break;
            case aFun::Parser::ParserEvent::parser_error_unknown:
            default:
                aFun::cout << "ERROR: Parser sends unknown error.";
                break;
        }
        aFun::cout << " (At " << file << ":" << event.line << ")\n";
    }
}

static void printMessage(const std::string &type, aFun::Message *msg, aFun::Inter &inter) {
    if (type == "NORMAL") {
        auto *msg_ = dynamic_cast<aFun::NormalMessage *>(msg);
        if (msg_ == nullptr)
            return;
        aFun::cout << "Inter result: " << msg_->getObject() <<"\n";
    } else if (type == "ERROR") {
        auto *msg_ = dynamic_cast<aFun::ErrorMessage *>(msg);
        if (msg_ == nullptr)
            return;
        int32_t error_std = 0;
        inter.getEnvVarSpace().findNumber("sys:error_std", error_std);
        if (error_std == 0) {
            aFuntool::printf_stderr(0, "Error TrackBack\n");
            for (auto &begin: msg_->getTrackBack())
                aFuntool::printf_stderr(0, "  File \"%s\", line %d\n", begin.path.c_str(), begin.line);
            aFuntool::printf_stderr(0, "%s: %s\n", msg_->getErrorType().c_str(), msg_->getErrorInfo().c_str());
        } else {
            aFuntool::printf_stdout(0, "Error TrackBack\n");
            for (auto &begin: msg_->getTrackBack())
                aFuntool::printf_stdout(0, "  File \"%s\", line %d\n", begin.path.c_str(), begin.line);
            aFuntool::printf_stdout(0, "%s: %s\n", msg_->getErrorType().c_str(), msg_->getErrorInfo().c_str());
        }
    }
}

void progressInterEvent(aFun::Inter &inter) {
    std::string type;
    for (auto msg = inter.getOutMessageStream().popFrontMessage(type); msg != nullptr; msg = inter.getOutMessageStream().popFrontMessage(type)) {
        printMessage(type, msg, inter);
        delete msg;
    }
}
