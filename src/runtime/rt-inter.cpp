#include "rt-inter.h"
#include "func-exit.h"
#include "func-import.h"

namespace aFunrt {
    aFunEnvironment::aFunEnvironment(int argc, char **argv) : Environment(argc, argv)  {
        {  // 退出函数
            auto exit = new ExitFunction(*this);
            protect->defineVar("exit", exit);
            exit->delReference();
        }

        {  // 导入函数
            auto import = new ImportFunction(*this);
            protect->defineVar("import", import);
            import->delReference();
        }
    }
}