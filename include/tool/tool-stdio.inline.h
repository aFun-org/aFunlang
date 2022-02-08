#ifndef AFUN_STDIO_INLINE_H
#define AFUN_STDIO_INLINE_H

#include "tool-stdio.h"

namespace aFuntool {
    bool clear_ferror(FILE *file) {
        return ferror(file) && (clearerr(file), ferror(file));
    }

    bool clear_stdin() {
        return (ferror(stdin) || feof(stdin)) &&
               (clearerr(stdin), (ferror(stdin) || feof(stdin)));
    }
}

#ifdef aFunWIN32_NO_CYGWIN
namespace aFuntool {
    int fputs_stdout(const char *str) {
        return fputs_std_(str, stdout);
    }

    int fputs_stderr(const char *str) {
        return fputs_std_(str, stderr);
    }

    size_t vprintf_stderr(size_t buf_len, const char *format, va_list ap) {
        return vprintf_std_(stderr, buf_len, format, ap);
    }
    size_t vprintf_stdout(size_t buf_len, const char *format, va_list ap) {
        return vprintf_std_(stdout, buf_len, format, ap);
    }

    size_t printf_stdout(size_t buf_len, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        size_t re = vprintf_std_(stdout, buf_len, format, ap);
        va_end(ap);
        return re;
    }

    size_t printf_stderr(size_t buf_len, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        size_t re = vprintf_std_(stderr, buf_len, format, ap);
        va_end(ap);
        return re;
    }
}
#else
namespace aFuntool {
    int fgetc_stdin(){
        return fgetc(stdout);
    }

    int fgets_stdin_(char *buf, int len, FILE *file){
        return fgets(buf, len, file) != nullptr;
    }

    int fungetc_stdin(char ch){
        return ungetc(ch, stdin);
    }

    int fputs_stdout(const char *str){
        return fputs(str, stdout);
    }

    int fputs_stderr(const char *str){
        return fputs(str, stderr);
    }

    int vprintf_stdout(size_t, const char *format, va_list ap){
        return vfprintf(stdout, format, ap);
    }

    int vprintf_stderr(size_t, const char *format, va_list ap){
        return vfprintf(stderr, format, ap);
    }

    size_t printf_stdout(size_t, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        size_t re = vfprintf(stdout, format, ap);
        va_end(ap);
        return re;
    }

    size_t printf_stderr(size_t, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        size_t re = vfprintf(stderr, format, ap);
        va_end(ap);
        return re;
    }
}

#endif

namespace aFuntool {
    OutStream::OutStream(PrintFunction *func_) : func {func_} {

    }

    OutStream &OutStream::operator<<(char a) {
        func(0, "%c", a);
        return *this;
    }

    OutStream &OutStream::operator<<(signed char a) {
        func(0, "%c", a);
        return *this;
    }

    OutStream &OutStream::operator<<(short a) {
        func(0, "%d", a);
        return *this;
    }

    OutStream &OutStream::operator<<(int a) {
        func(0, "%d", a);
        return *this;
    }

    OutStream &OutStream::operator<<(long a) {
        func(0, "%ld", a);
        return *this;
    }

    OutStream &OutStream::operator<<(long long a) {
        func(0, "%lld", a);
        return *this;
    }

    OutStream &OutStream::operator<<(unsigned char a) {
        func(0, "%c", a);
        return *this;
    }

    OutStream &OutStream::operator<<(unsigned short a) {
        func(0, "%u", a);
        return *this;
    }

    OutStream &OutStream::operator<<(unsigned int a) {
        func(0, "%u", a);
        return *this;
    }

    OutStream &OutStream::operator<<(unsigned long a) {
        func(0, "%lu", a);
        return *this;
    }

    OutStream &OutStream::operator<<(unsigned long long a) {
        func(0, "%llu", a);
        return *this;
    }

    OutStream &OutStream::operator<<(const char *a){
        func(0, "%s", a);
        return *this;
    }

    OutStream &OutStream::operator<<(const std::string &a) {
        func(0, "%s", a.c_str());
        return *this;
    }

    OutStream &OutStream::operator<<(const void *a) {
        func(0, "%p", a);
        return *this;
    }

    OutStream &OutStream::operator<<(float a) {
        func(0, "%f", a);
        return *this;
    }

    OutStream &OutStream::operator<<(double a) {
        func(0, "%f", a);
        return *this;
    }

    OutStream &OutStream::operator<<(long double a) {
        func(0, "%lf", a);
        return *this;
    }
}

#endif //AFUN_STDIO_INLINE_H
