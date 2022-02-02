#ifndef AFUN_MAIN_H
#define AFUN_MAIN_H
#include "aFunrt.h"
#include "fflags.h"

void printError(ff_FFlags *ff);

extern std::string program_name;
extern std::string home_path;
extern bool tty_stdin;

int mainHelp(ff_FFlags *ff);

void printVersion();
void printWelcomeInfo();
void printHelp();

int mainRun(ff_FFlags *ff);
int mainCL(ff_FFlags *ff);
int mainBuild(ff_FFlags *ff);

#endif //AFUN_MAIN_H
