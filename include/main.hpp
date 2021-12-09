﻿#ifndef AFUN_MAIN_HPP
#define AFUN_MAIN_HPP
#include "base.h"

#ifdef aFunWIN32_NO_CYGWIN
#include <io.h>
#define fileno _fileno
#define isatty _isatty
#else
#include "unistd.h"
#endif

extern char *base_path;
extern Logger *aFunlangLogger;

#endif //AFUN_MAIN_HPP