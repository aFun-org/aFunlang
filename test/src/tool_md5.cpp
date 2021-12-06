#include "tool.hpp"

int main(int argc, char **argv) {
    if (argc != 3)
        return 1;

    char *md5_answer = argv[1];
    char *file_path = argv[2];
    char *my_md5 = getFileMd5(file_path);

    if (EQ_STR(my_md5, md5_answer))
        return 0;
    return 1;  // 不相等, 表示错误
}
