#ifndef AFUN_STDIO_INLINE_H
#define AFUN_STDIO_INLINE_H

#include "tool-stdio.h"

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

namespace aFuntool {
    inline OutStream::OutStream(PrintFunction *func_) : func {func_} {

    }

    inline OutStream &OutStream::operator<<(int8_t a) {
        func(0, "%c", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(int16_t a) {
        func(0, "%d", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(int32_t a) {
        func(0, "%d", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(int64_t a) {
        func(0, "%ld", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(uint8_t a) {
        func(0, "%c", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(uint16_t a) {
        func(0, "%u", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(uint32_t a) {
        func(0, "%u", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(uint64_t a) {
        func(0, "%lu", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(const char *a){
        func(0, "%s", a);
        return *this;
    }

    inline OutStream &OutStream::operator<<(const std::string &a) {
        func(0, "%s", a.c_str());
        return *this;
    }

    inline OutStream &OutStream::operator<<(const void *a) {
        func(0, "%p", a);
        return *this;
    }
}

#endif //AFUN_STDIO_INLINE_H
