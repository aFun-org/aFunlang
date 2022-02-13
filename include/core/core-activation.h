#ifndef AFUN_CORE_ACTIVATION_H
#define AFUN_CORE_ACTIVATION_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core-message-stream.h"
#include "aFuncode.h"

namespace aFuncore {
    class Activation {
    public:
        typedef enum ActivationStatus {
            as_run = 0,
            as_end = 1,
            as_end_run = 2,
        } ActivationStatus;

        Activation() = default;
        virtual ~Activation() = default;
        Activation(const Activation &) = delete;
        Activation &operator=(Activation &) = delete;

        virtual ActivationStatus getCode(const aFuncode::Code::ByteCode *&code) = 0;
        virtual void runCode(const aFuncode::Code::ByteCode *code) = 0;
        virtual void endRun() = 0;

        [[nodiscard]] virtual UpMessageStream &getUpStream() = 0;
        [[nodiscard]] virtual DownMessageStream &getDownStream() = 0;

        [[nodiscard]] virtual aFuntool::FileLine getFileLine() = 0;
        [[nodiscard]] virtual const aFuntool::FilePath &getFilePath() = 0;
    };


}

#endif //AFUN_CORE_ACTIVATION_H
