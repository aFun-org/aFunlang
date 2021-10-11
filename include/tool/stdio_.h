#ifndef AFUN_STDIO__H
#define AFUN_STDIO__H
#include <stdio.h>

AFUN_TOOL_EXPORT int fgets_stdin(char **dest, int len);
AFUN_TOOL_EXPORT bool checkStdin(void);
AFUN_TOOL_EXPORT bool fclear_stdin(void);

#define CLEAR_FERROR(file) (ferror(file) && (clearerr(file), ferror(file)))  /* 出现错误后尝试修复, 并再次检查 */
#define CLEAR_STDIN(file) ((ferror(stdin) || feof(stdin)) && (clearerr(stdin), (ferror(stdin) || feof(stdin))))

#ifdef aFunWIN32_NO_CYGWIN
AFUN_TOOL_EXPORT int fgetc_stdin(void);
AFUN_TOOL_EXPORT char *fgets_stdin_(char *buf, size_t len);
AFUN_TOOL_EXPORT int fungetc_stdin(int ch);

AFUN_TOOL_EXPORT int fputs_stdout(char *str);
AFUN_TOOL_EXPORT int fputs_stderr(char *str);

AFUN_TOOL_EXPORT size_t vprintf_stdout(size_t buf_len, char *format, va_list ap);
AFUN_TOOL_EXPORT size_t vprintf_stderr(size_t buf_len, char *format, va_list ap);
AFUN_TOOL_EXPORT size_t printf_stdout(size_t buf_len, char *format, ...);
AFUN_TOOL_EXPORT size_t printf_stderr(size_t buf_len, char *format, ...);

#else
#define fgetc_stdin() fgetc(stdin)
#define fgets_stdin_(buf, len, stream) fgets(buf, len, stream)
#define fungetc_stdin(ch) ungetc((ch), stdin)

#define fputs_stdout(str) fputs((str), stdout)
#define fputs_stderr(str) fputs((str), stderr)

#define vprintf_stdout(buf_len, format, ap) vfprintf(stdout, (format), (ap))
#define vprintf_stderr(buf_len, format, ap) vfprintf(stderr, (format), (ap))
#define printf_stdout(buf_len, ...) fprintf(stdout, __VA_ARGS__)
#define printf_stderr(buf_len, ...) fprintf(stderr, __VA_ARGS__)

#endif
#endif //AFUN_STDIO__H
