#include "code.hpp"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    Code *start = new Code(1, "test.aun");
    start->connect(new Code("Test", 1))->connect(new Code(block_p, new Code(block_p, new Code("Test2", 2), 2), 2));
    start->displayAll();

    getEndian();

    FILE *file = fileOpen("text.aun", "wb");
    start->writeAll_v1(file);
    fileClose(file);

    start->destructAll();
    start = new Code(1, "test.aun");

    file = fileOpen("text.aun", "rb");
    start->readAll_v1(file);
    fileClose(file);

    printf("writr: \n");
    start->displayAll();
    start->destructAll();

    return 0;
}