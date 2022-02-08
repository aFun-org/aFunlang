#include <iostream>
#include "aFuntool.h"
using namespace aFuntool;

void exit_func_push1(void *) {
    std::cout << "I am exit push-1" << std::endl;
}

void exit_func_push2(void *) {
    std::cout << "I am exit push-2" << std::endl;
}

int main() {
    int exit_code = 0;
    try {
        aFunAtExit(exit_func_push1, nullptr);
        aFunAtExit(exit_func_push2, nullptr);
        aFunExit(0);
    } catch (aFuntool::Exit &e) {
        exit_code = e.getExitCode();
    }
    aFunExitReal(exit_code);
}
