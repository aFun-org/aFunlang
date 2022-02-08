#include <cstdio>
#include <cstdlib>
#include "__main.h"
#include "__fflags.h"

std::string program_name;
std::string home_path;
bool tty_stdin = false;

#ifdef aFunWIN32_NO_CYGWIN

int argc_s = 0;
char **argv_s = nullptr;

static void convertArgs(int argc, char *argv_ansi[]) {
    argc_s = argc;
    argv_s = aFun::safeCalloc<char *>((size_t)argc);
    for(int i = 0; i < argc; i++) {
        if (aFun::convertMultiByte(argv_s + i, argv_ansi[i], CP_ACP, CP_UTF8) == 0) {
            aFun::cerr << "Argument conversion error";
            exit(EXIT_FAILURE);
        }
    }
}

static void convertArgsFree(void *) {
    for(int i = 0; i < argc_s; i++)
        free(argv_s[i]);
    free(argv_s);
}

int main(int argc, char **argv_ansi) {
    convertArgs(argc, argv_ansi);
    aFun::aFunAtExit(convertArgsFree, nullptr);

    char **argv = argv_s;
#else
#include "unistd.h"

    int main(int argc, char **argv) {
#endif
    tty_stdin = isatty(fileno(stdin));
    home_path = aFun::getHomePath();
    if (home_path.empty()) {
        aFun::cerr << "aFunlang init error.";
        exit(EXIT_FAILURE);
    }

    try {
        auto factor = aFun::LogFactory(home_path + aFun::SEP + "aFunlog", true);

#ifdef aFunDEBUG
        auto core_logger = aFuntool::Logger(factor, "aFun-core", aFun::log_track);
        auto sys_logger = aFuntool::Logger(factor, "aFun-sys", aFun::log_track);
        auto aFun_logger = aFuntool::Logger(factor, "aFun", aFun::log_track);
#else
        auto core_logger = aFuntool::Logger(factor, "aFun-core", aFun::log_info);
        auto sys_logger = aFuntool::Logger(factor, "aFun-sys", aFun::log_info);
        auto aFun_logger = aFuntool::Logger(factor, "aFun", aFun::log_info);
#endif

        aFunit::aFunInitInfo info {home_path, factor, core_logger, sys_logger, aFun_logger};

        ff_FFlags *ff = ff_initFFlags(argc, argv, true, false, stderr, aFunlang_exe);
        if (ff == nullptr)
            aFun::aFunExit(EXIT_FAILURE);

        if (!aFunInit(&info)) {
            aFun::cerr << "aFunlang init error.";
            aFun::aFunExit(EXIT_FAILURE);
        }

        int exit_code = EXIT_SUCCESS;
        const char *child = ff_getChild(ff);
        program_name = *argv;  // 获取第一个参数为name

        if (!std::strcmp(child, "cl"))
            exit_code = mainCL(ff);
        else if (!std::strcmp(child, "build"))
            exit_code = mainBuild(ff);
        else if (!std::strcmp(child, "run"))
            exit_code = mainRun(ff);
        else
            exit_code = mainHelp(ff);

        ff_freeFFlags(ff);
        aFun::aFunExit(exit_code);
    } catch (aFun::Exit &e) {
        return e.getExitCode();
    }
}
