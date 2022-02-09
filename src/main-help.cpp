#include "__main.h"

void printVersion() {
    /* Version的显示不被翻译, 因为它通常还需要被其他程序读取而进行处理, 必须保证一致性 */
    aFun::cout << "aFunlang at      " << program_name << "\n";
    aFun::cout << "aFunlang home at " << home_path << "\n";
    aFun::cout << "version: " aFunVersion "\n";
    aFun::cout << aFunDescription "\n";
}

void printWelcomeInfo() {
    aFun::cout << "aFunlang " aFunVersion " CommandLine (" __DATE__ ", " __TIME__ ")\n";
    aFun::cout << "[" COMPILER_ID "] on " SYSTEM_NAME "\n";
    aFun::cout << "(Enter the aFun code to run in the top activity)\n";
}

void printHelp() {
    aFun::cout << "aFunlang Usage:\n";
}

int mainHelp(ff_FFlags *ff) {
    char *text = nullptr;
    int mark;
    bool have_opt = false;
    /* 无信号处理 */

    while (true) {
        mark = ff_getopt(&text, ff);

        switch (mark) {
            case 'h':
                printHelp();
                break;
            case 'v':
                printVersion();
                break;
            case -1:
                goto out;
            default:
                printError(ff);
                return EXIT_FAILURE;
        }

        have_opt = true;  // 表示有实际参数
    }
    out:
    if (!have_opt) {
        printHelp();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
