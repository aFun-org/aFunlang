#ifndef AFUN_MAIN_H
#define AFUN_MAIN_H
#include "aFunrt.h"
#include "fflags.h"

namespace aFun {
    using namespace aFuntool;
    using namespace aFuncore;
    using namespace aFunrt;
};

void printError(ff_FFlags *ff);
void printParserError(aFun::Parser &parser, const aFun::FilePath &file);
void progressInterEvent(aFun::Inter &inter);

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
