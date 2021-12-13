#include "code.hpp"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    Code *start = new Code(1, "test.aun");
    start->connect(new Code("Test", 1))->connect(new Code(block_p, new Code(block_p, new Code("Test3", 2), 2), 2));
    start->displayAll();
    std::string md5 = start->getMD5All_v1();
    printf("md5: %s\n", md5.c_str());

    getEndian();

    start->writeByteCode("test.aun");
    start->destructAll();
    start = new Code(1, "test.aun");

    start->readByteCode("test.aun");

    start->displayAll();
    md5 = start->getMD5All_v1();
    printf("md5: %s\n", md5.c_str());
    start->destructAll();

    return 0;
}