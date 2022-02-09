#include "aFuntool.h"
using namespace aFuntool;

int main(){
    int exit_code = 0;
    try {
        setlocale(LC_ALL, "");

        auto factor = LogFactory(std::string(".") + SEP + "aFunlog", true);
        auto logger = Logger(factor, "Test", aFuntool::log_info);
        infoLog(&logger, "Test logger");
        aFunExit(0);
    } catch (aFuntool::Exit &e) {
        exit_code = e.getExitCode();
    }
    aFunExitReal(exit_code);
}