#ifndef AFUN_TOOL_MACRO_H
#define AFUN_TOOL_MACRO_H

#ifdef __cplusplus
#define AFUN_STATIC [[maybe_unused]] static
#define AFUN_INLINE [[maybe_unused]] inline
#else
#ifdef _MSC_VER
#define AFUN_STATIC static
#define AFUN_INLINE inline
#else
#define AFUN_STATIC __attribute__((unused)) static
#define AFUN_INLINE __attribute__((unused)) inline
#endif
#endif

#define AFUN_NULL ((void)0)

#ifdef __cplusplus
#ifdef AFUN_TOOL_C
#define AFUN_TOOL_C_EXPORT_FUNC extern "C"
#define AFUN_TOOL_C_EXPORT_VAR  extern "C"
#else
#define AFUN_TOOL_C_EXPORT_FUNC AFUN_TOOL_EXPORT
#define AFUN_TOOL_C_EXPORT_VAR AFUN_TOOL_EXPORT extern
#endif
#else
#define AFUN_TOOL_C_EXPORT_FUNC AFUN_TOOL_EXPORT
#define AFUN_TOOL_C_EXPORT_VAR AFUN_TOOL_EXPORT extern
#endif

#endif //AFUN_TOOL_MACRO_H
