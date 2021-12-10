#ifndef AFUN_STDIO_H
#define AFUN_STDIO_H
#include <cstdio>
#include "base.h"
#include "aFunToolExport.h"

namespace aFuntool {
    AFUN_TOOL_EXPORT int fgets_stdin(char **dest, int len);
    AFUN_TOOL_EXPORT bool checkStdin();
    AFUN_TOOL_EXPORT bool fclear_stdin();
    static bool clear_ferror(FILE *file) {return ferror(file) && (clearerr(file), ferror(file));}
    static bool clear_stdin() {
        return (ferror(stdin) || feof(stdin)) &&
                (clearerr(stdin), (ferror(stdin) || feof(stdin)));
    }
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
    static int fputs_stdout(const char *str) {return fputs_std_(str, stdout);}
    static int fputs_stderr(const char *str) {return fputs_std_(str, stderr);}

    AFUN_TOOL_EXPORT size_t vprintf_std_(FILE *std, size_t buf_len, const char *format, va_list ap);
    static size_t vprintf_stderr(size_t buf_len, const char *format, va_list ap) {
        return vprintf_std_(stderr, buf_len, format, ap);
    }
    static size_t vprintf_stdout(size_t buf_len, const char *format, va_list ap) {
        return vprintf_std_(stdout, buf_len, format, ap);
    }

    static size_t printf_stdout(size_t buf_len, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        size_t re = vprintf_std_(stdout, buf_len, format, ap);
        va_end(ap);
        return re;
    }

    static size_t printf_stderr(size_t buf_len, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        size_t re = vprintf_std_(stderr, buf_len, format, ap);
        va_end(ap);
        return re;
    }
}

#else

namespace aFuntool {
    static int fgetc_stdin(){ return fgetc(stdout); }
    static int fgets_stdin_(char *buf, int len, FILE *file){ fgets(buf, len, file); }
    static int fungetc_stdin(char ch){ return ungetc(ch, stdin); }

    static int fputs_stdout(const char *str){ return fputs(str, stdout); }
    static int fputs_stderr(const char *str){ return fputs(str, stderr); }

    static int vprintf_stdout(size_t, const char *format, va_list ap){ return vfprintf(stdout, format, ap); }
    static int vprintf_stderr(size_t, const char *format, va_list ap){ return vfprintf(stderr, format, ap); }

    static size_t printf_stdout(size_t, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        size_t re = vfprintf(stdout, format, ap);
        va_end(ap);
        return re;
    }

    static size_t printf_stderr(size_t, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        size_t re = vfprintf(stderr, format, ap);
        va_end(ap);
        return re;
    }

}

#endif
#endif //AFUN_STDIO_H
