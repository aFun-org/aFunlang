﻿#ifndef AFUN_MAIN_BUILD_H_
#define AFUN_MAIN_BUILD_H_
#include "main.h"

int buildFileOutput(FilePath out, FilePath in, bool force);
int buildFileToPath(FilePath path, FilePath in, bool force);
int buildFileToSelf(FilePath in, bool force);

#endif //AFUN_MAIN_BUILD_H_
