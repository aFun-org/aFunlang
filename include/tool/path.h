#ifndef AFUN_PATH_H
#define AFUN_PATH_H

/* 路径工具 */
#ifdef __linux__

#define SEP "/"
#define SEP_CH '/'
#define SHARED_MARK ".so"

#else

#define SEP "\\"
#define SEP_CH '\\'
#define SHARED_MARK ".dll"

#endif

#endif //AFUN_PATH_H
