#ifndef AFUN_STDIO__H
#define AFUN_STDIO__H
#include <stdio.h>

AFUN_TOOL_EXPORT int fgets_stdin(char **dest, int len);
AFUN_TOOL_EXPORT bool checkStdin(void);
AFUN_TOOL_EXPORT bool fclear_stdin(void);

#define CLEAR_FERROR(file) (ferror(file) && (clearerr(file), ferror(file)))  /* 出现错误后尝试修复, 并再次检查 */
#define CLEAR_STDIN(file) ((ferror(stdin) || feof(stdin)) && (clearerr(stdin), (ferror(stdin) || feof(stdin))))

#ifdef aFunWIN32_NO_CYGWIN

AFUN_TOOL_EXPORT void stdio_signal_init(bool signal);
AFUN_TOOL_EXPORT bool stdio_check_signal(void);

#ifdef _MSC_VER
#pragma warning(disable : 5105)  // 关闭 5105 的警告输出 (Windows.h中使用)
#endif
#include <conio.h>
#include <io.h>
#include <Windows.h>

AFUN_TOOL_EXPORT int convertMultiByte(char **dest, char *str, UINT from, UINT to);  // win32 特有函数
AFUN_TOOL_EXPORT int convertWideByte(wchar_t **dest, char *str, UINT from);  // win32 特有函数
AFUN_TOOL_EXPORT int convertFromWideByte(char **dest, wchar_t *str, UINT to);

AFUN_TOOL_EXPORT int fgetc_stdin(void);
AFUN_TOOL_EXPORT char *fgets_stdin_(char *buf, size_t len);
AFUN_TOOL_EXPORT int fungetc_stdin(int ch);

AFUN_TOOL_EXPORT int fputs_std_(char *str, FILE *std);
#define fputs_stdout(std) fputs_std_(std, stdout)
#define fputs_stderr(std) fputs_std_(std, stderr)


AFUN_TOOL_EXPORT size_t vprintf_std_(FILE *std, size_t buf_len, char *format, va_list ap);
#define vprintf_stderr(len, format, ap) vprintf_std_(stderr, len, format, ap)
#define vprintf_stdout(len, format, ap) vprintf_std_(stdout, len, format, ap)


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
