#ifndef AFUN_STDIO_INLINE_H
#define AFUN_STDIO_INLINE_H

#include "tool_stdio.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    bool clear_ferror(FILE *file) {
        return ferror(file) && (clearerr(file), ferror(file));
    }

    bool clear_stdin() {
        return (ferror(stdin) || feof(stdin)) &&
               (clearerr(stdin), (ferror(stdin) || feof(stdin)));
    }

#ifndef AFUN_TOOL_C
}
#endif

#ifdef AFUN_WIN32_NO_CYGWIN
#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

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

#ifndef AFUN_TOOL_C
}
#endif

#else
#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

#ifndef __cplusplus
#define nullptr NULL
#endif

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

#ifdef __cplusplus
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

#else  // C 不允许省略形参名
    int vprintf_stdout(size_t _, const char *format, va_list ap){
        (void)_;  // 确保参数被使用
        return vfprintf(stdout, format, ap);
    }

    int vprintf_stderr(size_t _, const char *format, va_list ap){
        (void)_;  // 确保参数被使用
        return vfprintf(stderr, format, ap);
    }

    size_t printf_stdout(size_t _, const char *format, ...) {
        (void)_;  // 确保参数被使用
        va_list ap;
        va_start(ap, format);
        size_t re = vfprintf(stdout, format, ap);
        va_end(ap);
        return re;
    }

    size_t printf_stderr(size_t _, const char *format, ...) {
        (void)_;  // 确保参数被使用
        va_list ap;
        va_start(ap, format);
        size_t re = vfprintf(stderr, format, ap);
        va_end(ap);
        return re;
    }

#endif

#ifndef AFUN_TOOL_C
}
#endif

#endif

#ifdef __cplusplus
#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    OutStream::OutStream(PrintFunction *func_) noexcept : func {func_} {

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

#ifndef AFUN_TOOL_C
}
#endif
#endif  // __cplusplus

#endif //AFUN_STDIO_INLINE_H
