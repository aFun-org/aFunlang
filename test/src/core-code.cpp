#include "code.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    Code *start = Code::create(1, "test.aun");
    start->connect(Code::create("Test", 1))->connect(Code::create(block_p, Code::create(block_p, Code::create("Test3", 2), 2), 2));
    start->displayAll();
    std::string md5 = start->getMD5All_v1();
    printf("md5: %s\n", md5.c_str());

    getEndian();

    start->writeByteCode("test.aun");
    Code::destruct(start);
    start = Code::create(1, "test.aun");

    start->readByteCode("test.aun");

    start->displayAll();
    md5 = start->getMD5All_v1();
    printf("md5: %s\n", md5.c_str());
    Code::destruct(start);

    return 0;
}