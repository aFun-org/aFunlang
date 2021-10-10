/*
 * 文件名: stdio_.c
 * 目标: 用于终端输出的控制 (stdout, stdin, stderr)
 * 因为不同平台上终端采用的编码不同
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tool.h"

/* 注意:
 * checkStdin在Windows和Linux之前行为具有差别, 本质目标时检查缓冲区是否有内容
 * Linux使用无阻塞读取直接检查, 检查结果确实为缓冲区是否有内容
 * Windows则是通过记录每次读取时是否已经读取`\n`来判断缓冲区是否有内容, 并且配合khbit来判断是否有内容输入
 * 实际上, khbit只能代表有内容输入而无法确定内容是否已经输入到缓冲区中
 */

#ifdef aFunWIN32_NO_CYGWIN
#ifdef _MSC_VER
#pragma warning(disable : 5105)  // 关闭 5105 的警告输出 (Windows.h中使用)
#endif
#include <conio.h>
#include <io.h>
#include <Windows.h>
// 获取CodePage, 并将内存中utf-8字符串转换为对应编码输出
// cygwin环境下, 终端默认为uft-8

static bool stdin_empty = true;  // stdin读取内容遇到 \n, 用于检测stdin是否为空

static int convertMultiByte(char **dest, char *str, UINT from, UINT to) {
    if (str == NULL || dest == NULL)
        return 0;

    int tmp_len = MultiByteToWideChar(from, 0, str, -1, 0, 0);
    if (tmp_len == 0)
        return 0;

    wchar_t *tmp = calloc(tmp_len + 1, sizeof(wchar_t));
    if (MultiByteToWideChar(from, 0, str, -1, tmp, tmp_len) == 0)
        return 0;

    int dest_len = WideCharToMultiByte(to, 0, tmp, -1, NULL, 0, NULL, NULL);
    if (dest_len == 0)
        return 0;

    *dest = calloc(dest_len + 1, sizeof(char));
    int re = WideCharToMultiByte(to, 0, tmp, -1, *dest, dest_len, NULL, NULL);

    free(tmp);
    return re;
}

int fgets_stdin(char **dest, int len) {
    char *wstr = calloc(len, sizeof(char));
    int re = 0;

    if (!_isatty(fileno(stdin))) {
        *dest = NEW_STR(len);
        re = fgets(*dest, len, stdin) != NULL;
        if (!re)
            free(*dest);
        return re;
    }

    UINT code_page = GetConsoleCP();
    if (fgets(wstr, len, stdin) != NULL)
        re = convertMultiByte(dest, wstr, code_page, CP_UTF8);
    if (strchr(wstr, '\n') != NULL)
        stdin_empty = true;  // 没有读取到\n说明stdin还有内容
    else
        stdin_empty = false;  // 没有读取到\n说明stdin还有内容
    return re;
}

int fgetchar_stdin(void) {
    int ch = getc(stdin);
    if (ch == '\n')
        stdin_empty = true;
    else
        stdin_empty = false;
    return ch;
}

int fungetc_stdin(int ch) {
    int re = ungetc(ch, stdin);
    stdin_empty = false;
    return re;
}

/*
 * 函数名: checkStdin
 * 目标: 检查stdin缓冲区是否有内容
 * 有内容则返回true
 * 无内容则返回false
 */
bool checkStdin(void) {
    if (!_isatty(fileno(stdin)))
        return true;
    if (!stdin_empty)
        return true;
    return _kbhit();
}

static int fputs_std_(char *str, FILE *std) {
    UINT code_page = GetConsoleCP();
    char *wstr = NULL;
    int re = EOF;
    convertMultiByte(&wstr, str, CP_UTF8, code_page);
    if (wstr != NULL) {
        re = fputs(wstr, std);
        free(wstr);
    }
    return re;
}

int fputs_stdout(char *str) {
    if (_isatty(fileno(stdout)))
        return fputs_std_(str, stdout);
    fputs(str, stdout);
    return 1;
}

int fputs_stderr(char *str) {
    if (_isatty(fileno(stderr)))
        return fputs_std_(str, stderr);
    fputs(str, stderr);
    return 1;
}

static size_t vprintf_std_(FILE *std, size_t buf_len, char *format, va_list ap) {
    if (buf_len == 0)
        buf_len = 1024;
    buf_len += 10;  // 预留更多位置
    char *buf = calloc(buf_len, sizeof(char));
    size_t re = vsnprintf(buf, buf_len, format, ap);
    if (fputs_std_(buf, std) == EOF)
        re = 0;
    free(buf);
    return re;
}

size_t vprintf_stdout(size_t buf_len, char *format, va_list ap) {
    if (_isatty(fileno(stdout)))
        return vprintf_std_(stdout, buf_len, format, ap);
    return vfprintf(stdout, format, ap);
}

size_t vprintf_stderr(size_t buf_len, char *format, va_list ap) {
    if (_isatty(fileno(stderr)))
        return vprintf_std_(stderr, buf_len, format, ap);
    return vfprintf(stderr, format, ap);
}

size_t printf_stdout(size_t buf_len, char *format, ...) {
    va_list ap;
    va_start(ap, format);
    size_t re = vprintf_std_(stdout, buf_len, format, ap);
    va_end(ap);
    return re;
}

size_t printf_stderr(size_t buf_len, char *format, ...) {
    va_list ap;
    va_start(ap, format);
    size_t re = vprintf_std_(stderr, buf_len, format, ap);
    va_end(ap);
    return re;
}

#else
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// 用于Linux平台的IO函数
// 默认Linux平台均使用utf-8

int fgets_stdin(char **dest, int len) {
    *dest = calloc(len, sizeof(char));
    if (fgets(*dest, len, stdin) == NULL)
        return 0;
    return 1;
}
/*
 * 函数名: checkStdin
 * 目标: 检查stdin缓冲区是否有内容
 * 有内容则返回true
 * 无内容则返回false
 *
 * 参考自: https://gist.github.com/SuperH-0630/a4190b89d21c349a8d6882ca71453ae6
 */
bool checkStdin(void) {
    if (!isatty(fileno(stdin)))
        return true;
    bool re = false;

    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int ch = fgetc(stdin);
    CLEAR_FERROR(stdin);

    if (ch != EOF) {
        ungetc(ch, stdin);
        re = true;
    }

    fcntl(STDIN_FILENO, F_SETFL, oldf);
    return re;
}

#endif