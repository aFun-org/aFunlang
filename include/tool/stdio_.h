#ifndef AFUN_STDIO__H
#define AFUN_STDIO__H
#include <stdio.h>

AFUN_TOOL_EXPORT int fgets_stdin(char **dest, int len);

#ifdef aFunWIN32_NO_CYGWIN
AFUN_TOOL_EXPORT int fputs_stdout(char *str);
AFUN_TOOL_EXPORT int fputs_stderr(char *str);

AFUN_TOOL_EXPORT size_t vprintf_stdout(size_t buf_len, char *format, va_list ap);
AFUN_TOOL_EXPORT size_t vprintf_stderr(size_t buf_len, char *format, va_list ap);
AFUN_TOOL_EXPORT size_t printf_stdout(size_t buf_len, char *format, ...);
AFUN_TOOL_EXPORT size_t printf_stderr(size_t buf_len, char *format, ...);

#else
#define fputs_stdout(str) fputs((str), stdout)
#define fputs_stderr(str) fputs((str), stderr)

#define vprintf_stdout(buf_len, format, ap) vfprintf(stdout, (format), (ap))
#define vprintf_stderr(buf_len, format, ap) vfprintf(stderr, (format), (ap))
#define printf_stdout(buf_len, ...) fprintf(stdout, __VA_ARGS__)
#define printf_stderr(buf_len, ...) fprintf(stderr, __VA_ARGS__)

#endif
#endif //AFUN_STDIO__H
