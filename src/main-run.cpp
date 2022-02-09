#include "__main.h"
#include <thread>
#include <mutex>

static void runCodeThread(aFun::Inter &inter, aFun::Code &code, std::mutex &mutex, bool &is_end) {
    try {
        inter.runCode(code);
    } catch (std::exception &e) {
        errorLog(aFun::aFunLogger, "Exception not catch: %s", e.what());
    }

    std::unique_lock<std::mutex> lock{mutex};
    is_end = true;
}

static int runCode(aFun::Code &code, aFun::Environment &, aFun::Inter &inter) {
    std::mutex mutex;
    bool is_end = false;
    auto thread = std::thread(runCodeThread, std::ref(inter), std::ref(code), std::ref(mutex), std::ref(is_end));

    while (true) {
        {
            std::unique_lock<std::mutex> lock{mutex};
            if (is_end)
                break;
        }
        progressInterEvent(inter);
    }

    thread.join();
    progressInterEvent(inter);
    int exit_code = 0;
    inter.getEnvVarSpace().findNumber("sys:exit-code", exit_code);
    return exit_code;
}

int mainRun(ff_FFlags *ff) {
    char **argv = nullptr;
    int argc = ff_get_process_argv(&argv, ff);

    if (argc == 0) {
        return 0;
    } else {
        try {
            auto reader = aFun::FileReader(argv[0]);
            auto parser = aFun::Parser(reader);
            auto code = aFun::Code(argv[0]);
            bool ret = parser.parserCode(code);
            printParserError(parser, argv[0]);
            if (!ret)
                return EXIT_FAILURE;
            auto env = aFun::Environment(argc - 1, argv + 1);
            auto inter = aFun::Inter(env);
            return runCode(code, env, inter);
        } catch (aFun::readerFileOpenError &) {  // TODO: szh readerFileOpenError包含文件名
            aFun::cout << "Cannot open file: " << argv[0] << "\n";
            return EXIT_FAILURE;
        }
    }
}
