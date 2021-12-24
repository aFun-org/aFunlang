#include "inter.hpp"
#include "value.hpp"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto *inter = new Inter();
    auto *obj = new Object("Object", inter);
    inter->getGlobalVarlist()->defineVar("test-var", obj);

    auto *code = (new Code(0, "run-code.aun"));
    code->connect(new Code(block_p, new Code("test-var", 1), 0));
    inter->runCode(code);
    code->destructAll();
    delete inter;
    return 0;
}