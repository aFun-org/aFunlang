#ifndef AFUN_STDIO_H
#define AFUN_STDIO_H
#include <cstdio>
#include <cinttypes>
#include "aFunToolExport.h"
#include "macro.h"
#include "tool-type.h"

namespace aFuntool {
    AFUN_TOOL_EXPORT int fgets_stdin(char **dest, int len);
    AFUN_TOOL_EXPORT bool checkStdin();
    AFUN_TOOL_EXPORT bool fclear_stdin();
    AFUN_STATIC bool clear_ferror(FILE *file);
    AFUN_STATIC bool clear_stdin();
}

#ifdef AFUN_WIN32_NO_CYGWIN

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
    AFUN_STATIC int fputs_stdout(const char *str);
    AFUN_STATIC int fputs_stderr(const char *str);

    AFUN_TOOL_EXPORT size_t vprintf_std_(FILE *std, size_t buf_len, const char *format, va_list ap);

    AFUN_STATIC size_t vprintf_stderr(size_t buf_len, const char *format, va_list ap);
    AFUN_STATIC size_t vprintf_stdout(size_t buf_len, const char *format, va_list ap);

    AFUN_STATIC size_t printf_stdout(size_t buf_len, const char *format, ...);
    AFUN_STATIC size_t printf_stderr(size_t buf_len, const char *format, ...);
}

#else

namespace aFuntool {
    AFUN_STATIC int fgetc_stdin();
    AFUN_STATIC int fgets_stdin_(char *buf, int len, FILE *file);
    AFUN_STATIC int fungetc_stdin(char ch);

    AFUN_STATIC int fputs_stdout(const char *str);
    AFUN_STATIC int fputs_stderr(const char *str);

    AFUN_STATIC int vprintf_stdout(size_t, const char *format, va_list ap);
    AFUN_STATIC int vprintf_stderr(size_t, const char *format, va_list ap);

    AFUN_STATIC size_t printf_stdout(size_t, const char *format, ...);
    AFUN_STATIC size_t printf_stderr(size_t, const char *format, ...);
}

#endif

namespace aFuntool {
    class OutStream {
        typedef size_t PrintFunction(size_t, const char *, ...);
        PrintFunction *func;
    public:
        AFUN_INLINE explicit OutStream(PrintFunction *func_);
        AFUN_INLINE OutStream &operator<<(char a);
        AFUN_INLINE OutStream &operator<<(signed char a);
        AFUN_INLINE OutStream &operator<<(short a);
        AFUN_INLINE OutStream &operator<<(int a);
        AFUN_INLINE OutStream &operator<<(long a);
        AFUN_INLINE OutStream &operator<<(long long a);
        AFUN_INLINE OutStream &operator<<(unsigned char a);
        AFUN_INLINE OutStream &operator<<(unsigned short a);
        AFUN_INLINE OutStream &operator<<(unsigned int a);
        AFUN_INLINE OutStream &operator<<(unsigned long a);
        AFUN_INLINE OutStream &operator<<(unsigned long long a);
        AFUN_INLINE OutStream &operator<<(float a);
        AFUN_INLINE OutStream &operator<<(double a);
        AFUN_INLINE OutStream &operator<<(long double a);
        AFUN_INLINE OutStream &operator<<(const char *a);
        AFUN_INLINE OutStream &operator<<(const std::string &a);
        AFUN_INLINE OutStream &operator<<(const void *a);
    };

    AFUN_TOOL_EXPORT extern OutStream cout;
    AFUN_TOOL_EXPORT extern OutStream cerr;
}

#include "tool-stdio.inline.h"

#endif //AFUN_STDIO_H
