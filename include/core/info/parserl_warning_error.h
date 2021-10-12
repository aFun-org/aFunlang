/*
 * 文件名: parser_warning_error.h
 * 目标: 记录parser的警告和错误信息
 */
#ifndef AFUN_PARSERL_WARNING_ERROR_H
#define AFUN_PARSERL_WARNING_ERROR_H

/* 词法分析器错误和警告信息 */
#define IllegalCharLog "Get illegal characters"
#define IllegalCharConsole HT_aFunGetText(IllegalChar, "Get illegal characters")

#define IncompleteFileLog "Incomplete file" /* 文件不完整 */
#define IncompleteFileConsole HT_aFunGetText(LexicalIncompleteFile, "Incomplete file")

#define IncludeControlCharLog "Include control characters in the text (not recommended)" /* 文本中包含控制符 */
#define IncludeControlCharConsole HT_aFunGetText(LexicalIncludeCTRL, "Include control characters in the text (not recommended)")

/* 语法分析器错误和经过信息 */
#define CodeListStartErrorLog "CodeList did not get a suitable start symbol"
#define CodeListStartErrorConsole HT_aFunGetText(SyntacticCodeListStartError, "CodeList did not get a suitable start symbol")

#define CodeListEndErrorLog "CodeList did not get EOF/NUL with end"
#define CodeListEndErrorConsole HT_aFunGetText(SyntacticCodeListEndError, "CodeList did not get EOF/NUL with end")

#define CodeStartErrorLog "Code did not get a suitable start symbol"
#define CodeStartErrorConsole HT_aFunGetText(SyntacticCodeStartError, "Code did not get a suitable start symbol")

#define CodeBlockEndErrorLog "Code-Block did not get end block symbol with end"
#define CodeBlockEndErrorConsole HT_aFunGetText(SyntacticCodeBolckEndError, "Code-Block did not get end block symbol with end")

#define MakeCodeFailLog "Make code fail (Maybe by prefix)"
#define MakeCodeFailConsole HT_aFunGetText(SyntacticMakeCodeFail, "Make code fail (Maybe by prefix)")

#define MakeCodeFailLog "Make code fail (Maybe by prefix)"
#define MakeCodeFailConsole HT_aFunGetText(SyntacticMakeCodeFail, "Make code fail (Maybe by prefix)")

#define TooDeepLog "Recursion too deep"
#define TooDeepConsole HT_aFunGetText(SyntacticTooDeep, "Recursion too deep")

#define PrefixErrorLog "The system gets the prefix error"
#define PrefixErrorConsole HT_aFunGetText(SyntacticPrefixError, "The system gets the prefix error")

/* 封装Reader的错误 */
#define BOMErrorLog "Parser utf-8 with error BOM"
#define BOMErrorConsole HT_aFunGetText(ReaderBOMError, "Parser utf-8 with error BOM")

#define FileIOErrorLog "File IO error"
#define FileIOErrorConsole HT_aFunGetText(ReaderFileIOError, "File IO error")

#define StdinErrorLog "Stdin error/eof"
#define StdinErrorConsole HT_aFunGetText(ReaderStdinError, "Stdin error/eof")

#define TooMuchInputErrorLog "Too much input for stdin"
#define TooMuchInputErrorConsole HT_aFunGetText(TooMuchInputError, "Too much input for stdin")

#endif //AFUN_PARSERL_WARNING_ERROR_H
