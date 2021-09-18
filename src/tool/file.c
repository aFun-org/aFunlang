/*
 * 文件名: file.c
 * 目标: 关于文件读取的实用函数
 */

#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "tool.h"

#ifndef S_ISREG
#define	S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif

#ifndef S_ISDIR
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif

/*
 * 函数名: checkFile
 * 目标判断文件类型, 若是普通文件返回1, 若是文件夹返回2, 其他遇到错误返回0
 */
int checkFile(char *path){
    struct stat my_stat;
    if (path == NULL || stat(path, &my_stat) != 0)
        return 0;
    if (S_ISREG(my_stat.st_mode))  // 普通文件
        return 1;
    else if (S_ISDIR(my_stat.st_mode))
        return 2;
    else
        return 0;
}

/*
 * 函数: getFileName
 * 目标: 给定路径获取该路径所指定的文件名
 */
char *getFileName(char *path_1){
    char *slash = NULL;  // 名字开始的字符的指针
    char *point = NULL;  // 后缀名.所在的字符的指针
    char *name = NULL;  // 返回值
    char *path = strCopy(path_1);  // 复制数组, 避免path_1是常量字符串导致无法修改其值

    if (path[STR_LEN(path) - 1] == SEP_CH)  // 若路径的最后一个字符为SEP, 则忽略此SEP
        path[STR_LEN(path) - 1] = NUL;

    if ((slash = strrchr(path, SEP_CH)) == NULL)
        slash = path;
    else
        slash++;

    if ((point = strchr(path, '.')) != NULL)
        *point = NUL;

    name = strCopy(slash);
    free(path);

    if (!isalpha(*name) && *name != '_')
        name = strJoin("_", name, false, true);
    for (char *tmp = name; *tmp != 0; tmp++)
        if (!isalnum(*tmp) &&'_' != *tmp)
            *tmp = '_';
    return name;
}

/*
 * 函数名: fileNameToVar
 * 目标: 把一个文件名转换为合法的变量名(替换不合法字符为_)
 */
char *fileNameToVar(char *name, bool need_free){
    char *var;
    if (need_free)
        var = name;  // 在原数据上修改
    else
        var = strCopy(name);  // 复制新的数据再修改

    if (!isalpha(*var) && *var != '_')
        var = strJoin("_", var, false, true);
    for (char *tmp = var; *tmp != 0; tmp++)
        if (!isalnum(*tmp) &&'_' != *tmp)
            *tmp = '_';
    return var;
}

/*
 * 函数名: findPath
 * 目标: 转换路径为合法路径（相对路径->绝对路径, 绝对路径保持不变）
 */
#include "stdio.h"
char *findPath(char *path, char *env, bool need_free){
    assert(env[STR_LEN(env) - 1] == SEP_CH);  // env 必须以 SEP 结尾
#ifdef __linux
    if (path[0] != SEP_CH) { // 不以 / 开头
#else
    if (!(isupper(path[0]) && (path)[1] == ':')) {  // 不以盘符开头
#endif
        return strJoin(env, path, false, need_free);  // 调整为相对路径模式
    } else if (!need_free) { // 若设置了need_free为true, 则等于先复制在释放原来的, 等于没有复制， 所以不做操作
        return strCopy(path);
    } else {
        return path;
    }
}