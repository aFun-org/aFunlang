#include "tool.hpp"
using namespace aFuntool;

int main(int argc, char **argv){
    std::string base_path = getExedir(1);
    if (base_path.empty())
        aFunExit(0);

    log_factory.initLogSystem(base_path);
    aFunExit(0);
}