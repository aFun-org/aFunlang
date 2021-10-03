#include <stdio.h>
#include <stdlib.h>
#include "aFun.h"
#include "main_run.h"
#include "main_build.h"

ff_defArg(help, false)
                ff_argRule('h', help, not, 'h')
                ff_argRule('v', version, not, 'v')
ff_endArg(help, false);

ff_selfProcessChild(run, true);

ff_defArg(cl, false)
                ff_argRule('e', eval, must, 'e')
                ff_argRule('f', file, must, 'f')
                ff_argRule('s', source, must, 's')
                ff_argRule('b', byte, must, 'b')
                ff_argRule(NUL, no-aub, not, 'a')
                ff_argRule(NUL, no-cl, not, 'n')
                ff_argRule(NUL, no-import, not, 'o')
                ff_argRule(NUL, import, not, 'i')
ff_endArg(cl, false);

ff_defArg(build, false)
                ff_argRule('o', out, must, 'o')
                ff_argRule('p', path, must, 'p')
                ff_argRule('f', fource, not, 'f')
ff_endArg(build, false);

// exe 是指可执行程序, 而非仅PE的可执行程序
ff_childList(aFunlang_exe, ff_child(run), ff_child(help), ff_child(cl), ff_child(build));

static const char *name = NULL;

static int mainHelp(ff_FFlags *ff);
static void printVersion(void);
static void printHelp(void);
static int mainRun(ff_FFlags *ff);
static int mainCL(ff_FFlags *ff);
static int mainBuild(ff_FFlags *ff);

char *base_path = NULL;
static Logger aFunlangLogger_;
Logger *aFunlangLogger = &aFunlangLogger_;

void freeBaseName(void) {
    free(base_path);
}

int main(int argc, char **argv) {
    jmp_buf main_buf;
    base_path = getExedir(*argv, 1);
    if (base_path == NULL)
        goto INIT_ERROR;
    atexit(freeBaseName);

    if (setjmp(main_buf) == 1)
        return EXIT_FAILURE;

    aFunInitInfo info = {.base_dir=base_path,
#ifdef aFunDEBUG
                         .level=log_track,
#else
                         .level=log_info,
#endif
                         .buf=&main_buf,
    };

    if (!aFunInit(&info)) {
INIT_ERROR:
        printf_stderr(0, "aFunlang init error.");
        return EXIT_FAILURE;
    }

    initLogger(aFunlangLogger, "aFunlang-exe", info.level);
    aFunlangLogger->buf = &main_buf;
    writeDebugLog(aFunlangLogger, "aFunlang-exe init success");

    int exit_code = EXIT_SUCCESS;
    ff_FFlags *ff = ff_initFFlags(argc, argv, true, false, stderr, aFunlang_exe);
    if (ff == NULL)
        return EXIT_FAILURE;
    char *child = ff_getChild(ff);
    name = *argv;  // 获取第一个参数为name

    if (EQ_STR(child, "cl"))
        exit_code = mainCL(ff);
    else if (EQ_STR(child, "build"))
        exit_code = mainBuild(ff);
    else if (EQ_STR(child, "run"))
        exit_code = mainRun(ff);
    else
        exit_code = mainHelp(ff);

    ff_freeFFlags(ff);
    return exit_code;
}

static void printVersion(void) {
    /* Version的显示不被翻译, 因为它通常还需要被其他程序读取而进行处理, 必须保证一致性 */
    printf_stdout(0, "aFunlang at  %s\n", name);
    printf_stdout(0, "aFunlang dir %s\n", base_path);
    fputs_stdout("version: " aFunVersion "\n");
    fputs_stdout(aFunDescription"\n");
}

static void printWelcomeInfo(void) {
    printf_stdout(0, "aFunlang " aFunVersion " %s (" __DATE__ ", " __TIME__ ")\n", HT_aFunGetText(command_line_n, "CommandLine"));
    fputs_stdout("["compilerID"] on "systemName"\n");
    printf_stdout(0, "(%s)\n", HT_aFunGetText(command_line_tips, "Enter the aFun code to run in the top activity"));
}

static void printHelp(void) {
    printf_stdout(0, "aFunlang %s:\n", HT_aFunGetText(usage_n, "Usage"));
    printf_stdout(0, "%s\n", HT_aFunGetText(help_info, "<base-tr>"));
}

/*
 * 函数名: printError
 * 目标: 打印参数错误信息
 */
static void printError(ff_FFlags *ff) {
    writeErrorLog(aFunlangLogger, "%s argument error", ff_getChild(ff));
    printf_stdout(0, "%s (%s)\n", HT_aFunGetText(cl_arg_error_e, "Command line argument error"), ff_getChild(ff));
    printHelp();
}

static int mainHelp(ff_FFlags *ff) {
    char *text = NULL;
    int mark;
    bool have_opt = false;

    while (1) {
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

static int mainRun(ff_FFlags *ff) {
    int exit_code;
    char **argv = NULL;
    int argc = ff_get_process_argv(&argv, ff);
    af_Environment *env;

    if (argc == 0) {
        /* 进入命令行模式 */
        env = creatAFunEnvironment(0, NULL);
        printWelcomeInfo();
        do
            exit_code = runCodeFromStdin("stdin", env);
        while (isCoreExit(env) != 1);
    } else {
        env = creatAFunEnvironment(argc - 1, argv + 1);
        exit_code = runCodeFromFile(argv[0], true, 0, env);
        destructAFunEnvironment(env);
    }

    if (exit_code != 0)
        writeErrorLog(aFunlangLogger, "aFun exit code: %d", exit_code);
    else
        writeInfoLog(aFunlangLogger, "aFun exit code: %d", exit_code);
    printf_stdout(0, "aFun %s: %d\n", HT_aFunGetText(exit_code_n, "exit code"), exit_code);

    return exit_code;
}

static RunList *getRunList(ff_FFlags *ff, bool *command_line, bool *save_aub) {
    char *text = NULL;
    RunList *run_list = NULL;
    RunList **prl = &run_list;
    int mark;
    bool import = true;

    *command_line = true;
    *save_aub = true;

    while (1) {
        mark = ff_getopt(&text, ff);
        switch (mark) {
            case 'e':
                prl = pushRunList(makeStringRunList(text, import), prl);
                break;
            case 'f':
                prl = pushRunList(makeFileRunList(text, import), prl);
                break;
            case 's':
                prl = pushRunList(makeFileSourceRunList(text, import), prl);
                break;
            case 'b':
                prl = pushRunList(makeFileByteRunList(text, import), prl);
                break;
            case 'n':
                *command_line = false;
                break;
            case 'a':
                *save_aub = false;
                break;
            case 'o':
                import = false;
                break;
            case 'i':
                import = true;
                break;
            case -1:
                goto out;
            default:
                *command_line = false;  // 命令行也不启动
                printError(ff);
                freeAllRunList(run_list);
                return NULL;
        }
    }

out:
    while (ff_getopt_wild_before(&text, ff))
        prl = pushRunList(makeFileRunList(text, true), prl);
    return run_list;
}

static int mainCL(ff_FFlags *ff) {
    bool command_line = true;
    bool save_aub = true;
    int exit_code;
    RunList *rl = getRunList(ff, &command_line, &save_aub);

    if (rl == NULL && !command_line) {
        writeErrorLog(aFunlangLogger, "CL not file to run");
        printf_stderr(0, "%s\n", HT_aFunGetText(cl_not_file_e, "There are not file run"));
        return EXIT_FAILURE;
    }

    int argc = 0;
    char *text;
    while (ff_getopt_wild_after(&text, ff))
        argc++;

    char **argv = calloc(argc, sizeof(char *));
    for (int i = 0; ff_getopt_wild_after(&text, ff); i++)
        argv[i] = text;

    af_Environment *env = creatAFunEnvironment(argc, argv);
    if (rl != NULL)
        exit_code = runCodeFromRunList(rl, NULL, save_aub, env);

    if (command_line && isCoreExit(env) != 1) {
        printWelcomeInfo();
        do
            exit_code = runCodeFromStdin("stdin", env);
        while (isCoreExit(env) != 1);
    }

    if (exit_code != 0)
        writeErrorLog(aFunlangLogger, "aFun exit code: %d", exit_code);
    else
        writeInfoLog(aFunlangLogger, "aFun exit code: %d", exit_code);
    printf_stdout(0, "aFun %s: %d\n", HT_aFunGetText(exit_code_n, "exit code"), exit_code);

    destructAFunEnvironment(env);
    freeAllRunList(rl);
    free(argv);
    return exit_code;
}

static int mainBuild(ff_FFlags *ff) {
    char *text = NULL;
    char *out_put = NULL;
    char *path = NULL;
    bool force = false;
    int mark;

    while (1) {
        mark = ff_getopt(&text, ff);

        switch (mark) {
            case 'o':
                if (path != NULL) {
                    writeErrorLog(aFunlangLogger, "Build argument conflict");
                    printf_stderr(0, "%s (--out, --path)\n", HT_aFunGetText(arg_conflict_n, "Argument conflict"));
                    goto error;
                }
                out_put = text;
                break;
            case 'p':
                if (out_put != NULL) {
                    writeErrorLog(aFunlangLogger, "Build argument conflict");
                    printf_stderr(0, "%s (--out, --path)\n", HT_aFunGetText(arg_conflict_n, ""));
                    goto error;
                }
                path = text;
                break;
            case 'f':
                force = true;
                break;
            case -1:
                goto out;
            default:
                goto error;
        }
    }

out:
    if (out_put != NULL) {
        FilePath in = NULL;

        /* 如果没有参数 */
        if (!ff_getopt_wild(&text, ff)) {
            writeErrorLog(aFunlangLogger, "Build not source file");
            printf_stderr(0, "%s\n", HT_aFunGetText(build_not_src_e, "There are not source file to build"));
            goto error;
        } else
            in = text;

        /* 如果还有第二个参数 */
        if (ff_getopt_wild(&text, ff)) {
            writeErrorLog(aFunlangLogger, "Build too many source file");
            printf_stderr(0, "%s\n", HT_aFunGetText(build_many_src_e, "There are too many source file to build (Do not use --out option)"));
            goto error;
        }

        return buildFileOutput(out_put, in, force);
    } else if (path != NULL) {
        int exit_code = 0;
        while (ff_getopt_wild(&text, ff) && exit_code == 0)
            exit_code = buildFileToPath(path, text, force);
        return exit_code;
    }

    int exit_code = 0;
    while (ff_getopt_wild(&text, ff) && exit_code == 0)
        exit_code = buildFileToSelf(text, force);
    return exit_code;

error:
    printError(ff);
    return EXIT_FAILURE;
}