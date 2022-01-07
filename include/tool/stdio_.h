#ifndef AFUN_STDIO_H
#define AFUN_STDIO_H
#include <cstdio>
#include "tool.h"
#include "aFunToolExport.h"

namespace aFuntool {
    AFUN_TOOL_EXPORT int fgets_stdin(char **dest, int len);
    AFUN_TOOL_EXPORT bool checkStdin();
    AFUN_TOOL_EXPORT bool fclear_stdin();
    static bool clear_ferror(FILE *file);
    static bool clear_stdin();
}

#ifdef aFunWIN32_NO_CYGWIN

#ifdef _MSC_VER
#pragma warning(disable : 5105)  // 关闭 5105 的警告输出 (Windows.h中使用)
#endif
#include <conio.h>
#include <io.h>
#include <Windows.h>

namespace aFuntool {
    AFUN_TOOL_EXPORT void stdio_signal_init(bool signal);
    AFUN_TOOL_EXPORT bool stdio_check_signal();

    AFUN_TOOL_EXPORT int convertMultiByte(char **dest, const char *str, UINT from, UINT to);  // win32 特有函数
    AFUN_TOOL_EXPORT int convertWideByte(wchar_t **dest, const char *str, UINT from);  // win32 特有函数
    AFUN_TOOL_EXPORT int convertFromWideByte(char **dest, const wchar_t *str, UINT to);

    AFUN_TOOL_EXPORT int fgetc_stdin();
    AFUN_TOOL_EXPORT char *fgets_stdin_(char *buf, size_t len);
    AFUN_TOOL_EXPORT int fungetc_stdin(int ch);

    AFUN_TOOL_EXPORT int fputs_std_(const char *str, FILE *std);
    static int fputs_stdout(const char *str);
    static int fputs_stderr(const char *str);

    AFUN_TOOL_EXPORT size_t vprintf_std_(FILE *std, size_t buf_len, const char *format, va_list ap);

    static size_t vprintf_stderr(size_t buf_len, const char *format, va_list ap);
    static size_t vprintf_stdout(size_t buf_len, const char *format, va_list ap);

    static size_t printf_stdout(size_t buf_len, const char *format, ...);
    static size_t printf_stderr(size_t buf_len, const char *format, ...);
}

#else

namespace aFuntool {
    static int fgetc_stdin();
    static int fgets_stdin_(char *buf, int len, FILE *file);
    static int fungetc_stdin(char ch);

    static int fputs_stdout(const char *str);
    static int fputs_stderr(const char *str);

    static int vprintf_stdout(size_t, const char *format, va_list ap);
    static int vprintf_stderr(size_t, const char *format, va_list ap);

    static size_t printf_stdout(size_t, const char *format, ...);
    static size_t printf_stderr(size_t, const char *format, ...);
}

#endif

#include "stdio_.inline.h"

#endif //AFUN_STDIO_H
