#include "aFuntool.h"

int main(int argc, char **argv) {
    if (argc != 3)
        return 1;

    char *md5_answer = argv[1];
    char *file_path = argv[2];
    char *my_md5 = aFuntool::getFileMd5(file_path);

    bool ret = !strcmp(my_md5, md5_answer);
    aFuntool::safeFree(my_md5);

    if (ret)
        return 0;
    return 1;  // 不相等, 表示错误
}
