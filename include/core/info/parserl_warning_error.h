/*
 * 文件名: parser_warning_error.h
 * 目标: 记录parser的警告和错误信息
 */
#ifndef AFUN_PARSERL_WARNING_ERROR_H
#define AFUN_PARSERL_WARNING_ERROR_H

/* 词法分析器错误和警告信息 */
#define LEXICAL_ERROR(status, info) ("status: " #status " " info)

#define SYS_ILLEGAL_CHAR(status) LEXICAL_ERROR(status, "System error to obtain illegal characters") /* switch分支获得了不可能的字符 */
#define ILLEGAL_CHAR(status) LEXICAL_ERROR(status, "Illegal characters") /* 输入了非法字符 */

#define SYS_ERROR_STATUS(status) LEXICAL_ERROR(status, "System error to jump status") /* 状态跳转错误 */
#define INCOMPLETE_FILE(status) LEXICAL_ERROR(status, "Incomplete file") /* 文件不完整 */
#define INCULDE_CONTROL(status) LEXICAL_ERROR(status, "Include control characters in the text (not recommended)") /* 文本中包含控制符 */

/* 语法分析器错误和经过信息 */
#define SYNTACTIC_ERROR(status, info) (#status ": " info)

#define CodeListStartError() SYNTACTIC_ERROR(CodeList, "CodeList did not get a suitable start symbol")
#define CodeListEndError() SYNTACTIC_ERROR(CodeList, "CodeList did not get EOF/NUL with end")

#define CodeStartError() SYNTACTIC_ERROR(Code, "Code did not get a suitable start symbol")
#define CodeEndError(p) SYNTACTIC_ERROR(Code, "Code-Block did not get " p " with end")

#define MakeCodeFail() SYNTACTIC_ERROR(Code, "Make code fail (Maybe by prefix)")

#define SYNTACTIC_TOO_DEEP() SYNTACTIC_ERROR(Syntactic, "Recursion too deep")

#define PREFIX_ERROR(satus) SYNTACTIC_ERROR(status, "The system gets the prefix error")

#endif //AFUN_PARSERL_WARNING_ERROR_H
