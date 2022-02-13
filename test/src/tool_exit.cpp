#include <iostream>
#include "aFuntool.h"

void exit_func_push1(void *) {
    std::cout << "I am exit push-1" << std::endl;
}

void exit_func_push2(void *) {
    std::cout << "I am exit push-2" << std::endl;
}

int main() {
    int exit_code = 0;
    try {
        aFuntool::aFunAtExit(exit_func_push1, nullptr);
        aFuntool::aFunAtExit(exit_func_push2, nullptr);
        aFuntool::aFunExit(0);
    } catch (aFuntool::Exit &e) {
        exit_code = e.getExitCode();
    }
    aFuntool::aFunExitReal(exit_code);
}
