#ifndef AFUN_PATH_H
#define AFUN_PATH_H

/* 路径工具 */
#ifdef WIN32

#define SEP "\\"
#define SEP_CH '\\'

#else

#define SEP "/"
#define SEP_CH '/'

#endif

#ifdef aFunWIN32
#define SHARED_MARK ".dll"
#else
#define SHARED_MARK ".so"
#endif

#endif //AFUN_PATH_H
