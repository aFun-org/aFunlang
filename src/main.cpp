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
    argv_s = aFuntool::safeCalloc<char *>((size_t)argc);
    for(int i = 0; i < argc; i++) {
        if (aFuntool::convertMultiByte(argv_s + i, argv_ansi[i], CP_ACP, CP_UTF8) == 0) {
            aFuntool::cerr << "Argument conversion error";
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
    aFuntool::aFunAtExit(convertArgsFree, nullptr);

    char **argv = argv_s;
#else
    int main(int argc, char **argv) {
#endif
    tty_stdin = isatty(fileno(stdin));
    home_path = aFuntool::getExedir(1);
    if (home_path.empty()) {
        aFuntool::cerr << "aFunlang init error.";
        exit(EXIT_FAILURE);
    }

    try {
        aFuntool::LogFactory factor{};
        factor.initLogSystem(home_path + aFuntool::SEP + "aFunlog");

#ifdef aFunDEBUG
        aFunrt::aFunInitInfo info{home_path, factor, true, aFuntool::log_debug, aFuntool::log_debug};
#else
        aFunrt::aFunInitInfo info{base_path, factor, true, aFuntool::log_info, aFuntool::log_info};
#endif

        ff_FFlags *ff = ff_initFFlags(argc, argv, true, false, stderr, aFunlang_exe);
        if (ff == nullptr)
            aFuntool::aFunExit(EXIT_FAILURE);

        if (!aFunInit(&info)) {
            aFuntool::cerr << "aFunlang init error.";
            aFuntool::aFunExit(EXIT_FAILURE);
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
        aFuntool::aFunExit(exit_code);
    } catch (aFuntool::Exit &e) {
        return e.getExitCode();
    }
}
