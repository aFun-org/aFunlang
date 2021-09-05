/*
 * 文件名: lexical_warning_error.h
 * 目标: 记录lexical的警告和错误信息
 */
#ifndef AFUN_LEXICAL_WARNING_ERROR_H
#define AFUN_LEXICAL_WARNING_ERROR_H

#define LEXICAL_ERROR(status, info) ("status: " #status " " #info)

#define SYS_ILLEGAL_CHAR(status) LEXICAL_ERROR(status, "System error to obtain illegal characters") /* switch分支获得了不可能的字符 */
#define ILLEGAL_CHAR(status) LEXICAL_ERROR(status, "Illegal characters") /* 输入了非法字符 */

#define SYS_ERROR_STATUS(status) LEXICAL_ERROR(status, "System error to jump status") /* 状态跳转错误 */
#define INCOMPLETE_FILE(status) LEXICAL_ERROR(status, "Incomplete file") /* 文件不完整 */
#define INCULDE_CONTROL(status) LEXICAL_ERROR(status, "Include control characters in the text (not recommended)") /* 文本中包含控制符 */

#endif //AFUN_LEXICAL_WARNING_ERROR_H
