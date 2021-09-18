#ifndef AFUN_FILE_H
#define AFUN_FILE_H

/* 文件处理工具 */
int checkFile(char *path);
char *getFileName(char *path_1);
char *fileNameToVar(char *name, bool need_free);
char *findPath(char *path, char *env, bool need_free);

#endif //AFUN_FILE_H
