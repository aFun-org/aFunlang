#ifndef AFUN_STDIO_INLINE_H
#define AFUN_STDIO_INLINE_H

#include "stdio_.h"

namespace aFuntool {
    static bool clear_ferror(FILE *file) {
        return ferror(file) && (clearerr(file), ferror(file));
    }

    static bool clear_stdin() {
        return (ferror(stdin) || feof(stdin)) &&
               (clearerr(stdin), (ferror(stdin) || feof(stdin)));
    }
}

#ifdef aFunWIN32_NO_CYGWIN
namespace aFuntool {
    static int fputs_stdout(const char *str) {
        return fputs_std_(str, stdout);
    }

    static int fputs_stderr(const char *str) {
        return fputs_std_(str, stderr);
    }

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
    static int fgetc_stdin(){
        return fgetc(stdout);
    }

    static int fgets_stdin_(char *buf, int len, FILE *file){
        return fgets(buf, len, file) != nullptr;
    }

    static int fungetc_stdin(char ch){
        return ungetc(ch, stdin);
    }

    static int fputs_stdout(const char *str){
        return fputs(str, stdout);
    }

    static int fputs_stderr(const char *str){
        return fputs(str, stderr);
    }

    static int vprintf_stdout(size_t, const char *format, va_list ap){
        return vfprintf(stdout, format, ap);
    }

    static int vprintf_stderr(size_t, const char *format, va_list ap){
        return vfprintf(stderr, format, ap);
    }

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
#endif //AFUN_STDIO_INLINE_H
