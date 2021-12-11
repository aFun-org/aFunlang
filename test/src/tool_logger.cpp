#include "tool.hpp"
using namespace aFuntool;

int main(int argc, char **argv){
    std::string base_path = getExedir(1);
    if (base_path.empty()) {
        printf("Not Exe Dir\n");
        aFunExit(0);
    }

    setlocale(LC_ALL, "");
    log_factory.initLogSystem(base_path + SEP + "aFunlog");
    aFunExit(0);
}