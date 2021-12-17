#include "inter.hpp"
#include "value.hpp"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto *inter = new Inter();
    auto *obj = new Object("Object", inter);
    inter->defineGlobalVar("test-var", obj);

    std::cout << "test-var = " << inter->findGlobalObject("test-var")
              << " non-var = " << inter->findGlobalObject("non-var")
              << " obj = " << obj
              << std::endl;

    auto *code = (new Code(0, "run-code.aun"));
    code->connect(new Code("test-var", 1));

    code->destructAll();
    delete inter;
    return 0;
}