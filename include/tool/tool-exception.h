#ifndef AFUN_TOOL_EXCEPTION_H
#define AFUN_TOOL_EXCEPTION_H
#ifdef __cplusplus

#include "aFunToolExport.h"
#include "tool-macro.h"
#include "tool.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    class AFUN_TOOL_EXPORT aFunException : public std::exception {
        std::string message;
    public:
        AFUN_INLINE explicit aFunException(std::string msg);
        [[nodiscard]] const char *what() const noexcept override;
        [[nodiscard]] AFUN_INLINE const std::string &getMessage() const;
    };

    class aFuntoolException : public aFunException {
    public:
        AFUN_INLINE explicit aFuntoolException(const std::string &msg);
    };

    class FileOpenException : public aFuntoolException {
    public:
        AFUN_INLINE explicit FileOpenException(const FilePath &file);
    };

    class RegexException : public aFuntoolException {
    public:
        AFUN_INLINE explicit RegexException(const std::string &msg);
    };

    class LogFatalError : public aFuntoolException {
    public:
        AFUN_INLINE explicit LogFatalError(const char *msg);
    };

    class Exit : public aFuntoolException {
        int exit_code;
    public:
        AFUN_INLINE explicit Exit(int exit_code_);
        AFUN_INLINE int getExitCode() const;
    };

#ifndef AFUN_TOOL_C
}
#endif

#include "tool-exception.inline.h"

#endif
#endif //AFUN_TOOL_EXCEPTION_H
