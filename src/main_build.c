#include "aFun.h"
#include "main_build.h"

int buildFileOutput(FilePath out, FilePath in, bool force) {
    if (!force) {
        time_t time_1 = getFileMTime(in);
        time_t time_2 = getFileMTime(out);

        if (time_1 == 0 && time_2 == 0) {
            writeErrorLog(aFunlangLogger, log_default, "%s [%s]", HT_getText(NOT_BUILD_FILE_EXISITS, "File not exists"), in);
            return -1;
        }

        if (time_2 >= time_1) {
            writeErrorLog(aFunlangLogger, log_default, "%s (%s), %s", HT_getText(ALREADY_BUILD, "Source already build"), in, HT_getText(BUILD_AGAIN, "use --force to build again"));
            return 0;
        }
    }

    writeInfoLog(aFunlangLogger, log_default, "%s (%s -> %s)", HT_getText(BUILD_FILE, "File will be build"), in, out);
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
