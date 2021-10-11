﻿/*
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

#define BUFF_SIZE (40960)
static char buffer[BUFF_SIZE] = "";
static size_t index = 0;
static size_t next = 0;
static size_t end = 0;

static int setCursorPosition(HANDLE std_o, CONSOLE_SCREEN_BUFFER_INFO *info_, SHORT x_) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (info_ == NULL) {
        if (!GetConsoleScreenBufferInfo(std_o, &info))
            return -1;
        info_ = &info;
    }
    int x = info_->dwCursorPosition.X;
    int y = info_->dwCursorPosition.Y;

    x += x_;
    while (x >= info_->dwSize.X) {
        x -= info_->dwSize.X;
        y++;
    }

    while (x < 0) {
        x += info_->dwSize.X;
        y--;
    }

    if (y < 0)
        y = 0;
    else if (y > info_->dwSize.Y)
        y = info_->dwSize.Y;

    SetConsoleCursorPosition(std_o, (COORD){.X=(SHORT)x, .Y=(SHORT)y});
    return 1;
}

static int nextToEnd(HANDLE *std_o) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(std_o, &info))
        return 0;
    if (setCursorPosition(std_o, &info, (SHORT)(end - next)) == -1)
        return 0;
    next = end;
    return 1;
}

static int moveBuffer(void) {
    if (index == 0)
        return 0;
    memmove(buffer, buffer + index, BUFF_SIZE - index);
    end = end - index;
    next = next - index;
    index = 0;
    memset(buffer + end, 0, BUFF_SIZE - end);
    return 1;
}

static int backChar(HANDLE std_o) {
    if (index != next) {  // 删除一个字符
        if (setCursorPosition(std_o, NULL, -1) == -1)  // 先一定位置在-1
            return 0;

        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(std_o, &info))
            return 0;
        memmove(buffer + next - 1, buffer + next, end - next + 1);

        SetConsoleCursorPosition(std_o, info.dwCursorPosition);
        for (size_t n = next - 1; n < end; n++)
            fputc(' ', stdout);

        SetConsoleCursorPosition(std_o, info.dwCursorPosition);
        fputs(buffer + next - 1, stdout);

        SetConsoleCursorPosition(std_o, info.dwCursorPosition);
        next--;
        end--;
    }
    return 1;
}

static int enterChar(HANDLE std_o) {
    if(!nextToEnd(std_o))
        return 0;
    buffer[end] = '\n';
    end++;
    next++;
    fputc('\n', stdout);
    return 1;
}

/*
 * 函数名: newChar
 * 目标: 记录字符并显示
 * 返回1表示成功
 * 返回0表示失败
 */
static int newChar(HANDLE std_i, char ch) {
    if (ch == 0)
        return 1;
    if (end == BUFF_SIZE && !moveBuffer())  // 对比 end 而不是 next
        return 0;

    if (next != end) {  // insert 模式
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(std_i, &info))
            return 0;
        memmove(buffer + next + 1, buffer + next, end - next);
        buffer[next] = ch;
        fputs(buffer + next, stdout);
        if (setCursorPosition(std_i, &info, 1) == -1)
            return 0;
    } else {
        buffer[next] = ch;
        fputc(ch, stdout);
    }

    next++;
    end++;
    return 1;
}

/*
 * 函数名: checkNewInput
 * 目标: 获取输入并且显示
 * 返回-1表示遭遇错误
 * 返回0表示未完成行读取
 * 返回1表示完成行读取
 */
static int checkNewInput(HANDLE *std_i, HANDLE *std_o) {
    DWORD len = 0;
    DWORD oldm;
    if (!GetConsoleMode(std_i, &oldm))
        return -1;

    if (!GetNumberOfConsoleInputEvents(std_i, &len))
        return -1;

    for (int i = 0; i < len; i++) {
        INPUT_RECORD record;
        DWORD read_len;
        if (!ReadConsoleInputA(std_i, &record, 1, &read_len) || read_len == 0)
            return -1;
        if (record.EventType == KEY_EVENT) {
            if (!record.Event.KeyEvent.bKeyDown)
                continue;
            else if (record.Event.KeyEvent.wVirtualKeyCode == VK_BACK) {  // 退格
                if (backChar(std_o) == 0)
                    return -1;
                continue;
            } if (record.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {  // 回车
                if (enterChar(std_o) == 0)
                    return -1;
                return 1;
            } else if (record.Event.KeyEvent.wVirtualKeyCode == VK_LEFT) {  // 左
                CONSOLE_SCREEN_BUFFER_INFO info;
                if (!GetConsoleScreenBufferInfo(std_o, &info))
                    return -1;
                if (next > index) {
                    next--;
                    if (setCursorPosition(std_o, NULL, -1) == -1)
                        return 0;
                }
                return 0;
            } else if (record.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT) {  // 右
                if (next < end) {
                    next++;
                    if(setCursorPosition(std_o, NULL, 1) == -1)
                        return 0;
                }
                return 0;
            }

            for (int r = record.Event.KeyEvent.wRepeatCount; r > 0; r--) {
                if (newChar(std_o, record.Event.KeyEvent.uChar.AsciiChar) == 0)
                    return -1;
            }
        }
    }
    return 0;
}

static int fcheck_stdin(HANDLE *std_i, HANDLE *std_o) {
    if (end == index || end == 0 || buffer[end - 1] != '\n')
        return checkNewInput(std_i, std_o);
    return 1;
}

int fgetc_stdin(void) {
    HANDLE *std_i = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE *std_o = GetStdHandle(STD_OUTPUT_HANDLE);
    if (std_i == INVALID_HANDLE_VALUE || std_o == INVALID_HANDLE_VALUE)
        return EOF;

    for (int fs = 0; fs != 1 ; fs = fcheck_stdin(std_i, std_o)) {  // 阻塞
        if (fs == -1)
            return EOF;
    }

    int re = (int)buffer[index];
    index++;
    return re;
}

char *fgets_stdin_(char *buf, size_t len) {
    HANDLE *std_i = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE *std_o = GetStdHandle(STD_OUTPUT_HANDLE);
    if (std_i == INVALID_HANDLE_VALUE || std_o == INVALID_HANDLE_VALUE)
        return NULL;

    for (int fs = 0; fs != 1 ; fs = fcheck_stdin(std_i, std_o)) {  // 阻塞
        if (fs == -1)
            return NULL;
    }

    size_t len_ = len - 1;
    if (end - index < len_)
        len_ = end - index;
    memcpy(buf, buffer+index, len_);
    index += len_;
    nextToEnd(std_o);
    buf[len_] = '\0';  // 最后一位
    return buf;
}

bool fclear_stdin(void) {
    HANDLE *std_o = GetStdHandle(STD_OUTPUT_HANDLE);
    if (std_o == INVALID_HANDLE_VALUE)
        return true;
    nextToEnd(std_o);
    index = 0;
    end = 0;
    next = 0;
    memset(buffer, 0, BUFF_SIZE);
    return false;
}

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

    if (!_isatty(_fileno(stdin))) {
        *dest = NEW_STR(len);
        re = fgets(*dest, len, stdin) != NULL;
        if (!re)
            free(*dest);
        return re;
    }

    UINT code_page = GetConsoleCP();
    if (fgets_stdin_(wstr, len) != NULL)
        re = convertMultiByte(dest, wstr, code_page, CP_UTF8);
    return re;
}

int fungetc_stdin(int ch) {
    if (ch == 0 || index == 0 && end == BUFF_SIZE)
        return 0;

    if (index != 0) {
        index--;
        buffer[index] = (char)ch;
    } else if (end != BUFF_SIZE) {  // index == 0;
        memmove(buffer, buffer + 1, end);  // 往回移动
        end++;
        next++;
        buffer[0] = (char) ch;
    }
    return 1;
}

/*
 * 函数名: checkStdin
 * 目标: 检查stdin缓冲区是否有内容
 * 有内容则返回true
 * 无内容则返回false
 */
bool checkStdin(void) {
    HANDLE *std_i = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE *std_o = GetStdHandle(STD_OUTPUT_HANDLE);
    int fs = fcheck_stdin(std_i, std_o);
    if (fs == 0)
        return false;
    return true;
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
    if (_isatty(_fileno(stdout)))
        return fputs_std_(str, stdout);
    fputs(str, stdout);
    return 1;
}

int fputs_stderr(char *str) {
    if (_isatty(_fileno(stderr)))
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
    if (_isatty(_fileno(stdout)))
        return vprintf_std_(stdout, buf_len, format, ap);
    return vfprintf(stdout, format, ap);
}

size_t vprintf_stderr(size_t buf_len, char *format, va_list ap) {
    if (_isatty(_fileno(stderr)))
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

bool fclear_stdin(void) {
    if (!isatty(fileno(stdin)))
        return true;
    bool re = false;

    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int ch;
    do {
        ch = fgetc(stdin);
        CLEAR_FERROR(stdin);
    } while (ch != EOF);

    fcntl(STDIN_FILENO, F_SETFL, oldf);
    return !ferror(stdin) && !feof(stdin);
}

#endif