#include "aFun.h"
#include "main_build.h"

int buildFileOutput(FilePath out, FilePath in, bool force) {
    if (!force) {
        time_t time_1 = getFileMTime(in);
        time_t time_2 = getFileMTime(out);

        if (time_1 == 0 && time_2 == 0) {
            writeErrorLog(aFunlangLogger, "Source not exists: %s", in);
            printf_stderr(0, "%s [%s]\n", HT_getText(build_src_not_exists_e, "Source not exists"), in);
            return -1;
        }

        if (time_2 >= time_1) {
            writeWarningLog(aFunlangLogger, "Source already build %s", in);
            printf_stderr("%s (%s), %s\n", HT_getText(build_src_already, "Source already build"), in, HT_getText(build_use_f, "use --force to build again"));
            return 0;
        }
    }

    writeInfoLog(aFunlangLogger, "Build %s, %s",  in, out);
    printf_stdout("Source will be build (%s -> %s)\n", HT_getText(build_file, "Source will be build"), in, out);
    return buildFile(out, in);
}

int buildFileToPath(FilePath path, FilePath in, bool force) {
    char *name = getFileName(in);
    char *out = joinPath(path, name, ".aub");

    int res = buildFileOutput(out, in, force);

    free(name);
    free(out);
    return res;
}

int buildFileToSelf(FilePath in, bool force) {
    char *path = getFileNameWithPath(in);
    char *out = strJoin(path, ".aub", true, false);

    int res = buildFileOutput(out, in, force);

    free(out);
    return res;
}
