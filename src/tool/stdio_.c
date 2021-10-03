/*
 * 文件名: stdio_.c
 * 目标: 用于终端输出的控制 (stdout, stdin, stderr)
 * 因为不同平台上终端采用的编码不同
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tool.h"

#ifdef aFunWIN32_NO_CYGWIN
#ifdef _MSC_VER
#pragma warning(disable : 5105)  // 关闭 5105 的警告输出 (Windows.h中使用)
#endif
#include <Windows.h>
// 获取CodePage, 并将内存中utf-8字符串转换为对应编码输出
// cygwin环境下, 终端默认为uft-8

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

    UINT code_page = GetConsoleCP();
    if (fgets(wstr, len, stdin) != NULL)
        re = convertMultiByte(dest, wstr, code_page, CP_UTF8);
    return re;
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
    return fputs_std_(str, stdout);
}

int fputs_stderr(char *str) {
    return fputs_std_(str, stderr);
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
    return vprintf_std_(stdout, buf_len, format, ap);
}

size_t vprintf_stderr(size_t buf_len, char *format, va_list ap) {
    return vprintf_std_(stderr, buf_len, format, ap);
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
// 用于Linux平台的IO函数
// 默认Linux平台均使用utf-8

int fgets_stdin(char **dest, int len) {
    *dest = calloc(len, sizeof(char));
    if (fgets(*dest, len, stdin) == NULL)
        return 0;
    return 1;
}

#endif