#include "aFuntool.h"
using namespace aFuntool;

int main(int argc, char **argv){
    int exit_code = 0;
    try {
        std::string base_path = getExedir(1);
        if (base_path.empty()) {
            printf("Not Exe Dir\n");
            aFunExit(0);
        }

        setlocale(LC_ALL, "");

        static LogFactory factor{};
        factor.initLogSystem(base_path + SEP + "aFunlog");

        static auto logger = Logger(factor, "Test", aFuntool::log_info);
        infoLog(&logger, "Test logger");
        aFunExit(0);
    } catch (aFuntool::Exit &e) {
        exit_code = e.getExitCode();
    }
    aFunExitReal(exit_code);
}