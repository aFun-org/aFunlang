#ifndef AFUN_LOG_M_HPP
#define AFUN_LOG_M_HPP

#if (defined aFunOFFAllLog || defined aFunOFFLog)

#undef aFunWriteTrack
#define aFunWriteTrack 0  /* 必须忽略Track */

#undef aFunWriteTrack
#define aFunWriteDebug 0  /* 必须忽略Debug */

#undef aFunWriteTrack
#define aFunWriteInfo 0  /* 必须忽略Info */

#undef aFunWriteTrack
#define aFunIgnoreWarning 0  /* 必须忽略Warning */

#undef aFunWriteTrack
#define aFunIgnoreError 0  /* 必须忽略Error */

#undef aFunWriteTrack
#define aFunIgnoreSendError 0  /* 必须忽略SendError */

#undef aFunWriteTrack
#define aFunIgnoreFatalError 0  /* 必须忽略FatalError */

#undef aFunWriteTrack
#define aFunIgnoreFatalError 0  /* 必须忽略FatalError */

#elif defined aFunDEBUG

#ifndef aFunWriteTrack
#define aFunWriteTrack 1  /* 是否记录Track */
#endif

#ifndef aFunWriteDebug
#define aFunWriteDebug 1  /* 是否记录Debug */
#endif

#ifndef aFunWriteInfo
#define aFunWriteInfo 1  /* 是否记录Info */
#endif

#ifndef aFunIgnoreWarning
#define aFunIgnoreWarning 0  /* 是否忽略Warning */
#endif

#ifndef aFunIgnoreError
#define aFunIgnoreError 0  /* 是否忽略Error */
#endif

#ifndef aFunIgnoreSendError
#define aFunIgnoreSendError 0  /* 是否忽略SendError */
#endif

#ifndef aFunIgnoreFatalError
#define aFunIgnoreFatalError 0  /* 是否忽略FatalError */
#endif

#else

#undef aFunWriteTrack
#define aFunWriteTrack 0  /* 必须忽略Track */

#ifndef aFunWriteDebug
#define aFunWriteDebug 1  /* 是否记录Debug */
#endif

#undef aFunConsoleDebug
#define aFunConsoleDebug 0

#ifndef aFunWriteInfo
#define aFunWriteInfo 1  /* 是否记录Info */
#endif

#undef aFunConsoleInfo
#define aFunConsoleInfo 0

#ifndef aFunIgnoreWarning
#define aFunIgnoreWarning 0  /* 是否忽略Warning */
#endif

#ifndef aFunIgnoreError
#define aFunIgnoreError 0  /* 是否忽略Error */
#endif

#ifndef aFunIgnoreSendError
#define aFunIgnoreSendError 0  /* 是否忽略SendError */
#endif

#ifndef aFunIgnoreFatalError
#define aFunIgnoreFatalError 0  /* 是否忽略FatalError */
#endif

#endif

#if defined aFunDEBUG
#undef aFunConsoleTrack
#define aFunConsoleTrack 1
#elif (!aFunWriteTrack || !defined aFunConsoleTrack)
#undef aFunConsoleTrack
#define aFunConsoleTrack 0
#endif

#if !aFunWriteDebug
#undef aFunConsoleDebug
#define aFunConsoleDebug 0
#elif !defined aFunConsoleDebug
#define aFunConsoleDebug 1
#endif

#if !aFunWriteInfo
#undef aFunConsoleInfo
#define aFunConsoleInfo 0
#elif !defined aFunConsoleInfo
#define aFunConsoleInfo 1
#endif

#if aFunIgnoreWarning
#undef aFunConsoleWarning
#define aFunConsoleWarning 0
#elif !defined aFunConsoleWarning
#define aFunConsoleWarning 1
#endif

#if aFunIgnoreError
#undef aFunConsoleError
#define aFunConsoleError 0
#elif !defined aFunConsoleError
#define aFunConsoleError 1
#endif

#if aFunIgnoreSendError
#undef aFunConsoleSendError
#define aFunConsoleSendError 0
#elif !defined aFunConsoleSendError
#define aFunConsoleSendError 1
#endif

#if aFunIgnoreFatalError
#undef aFunConsoleFatalError
#define aFunConsoleFatalError 0
#elif !defined aFunConsoleFatalError
#define aFunConsoleFatalError 1
#endif

#endif //AFUN_LOG_M_HPP
