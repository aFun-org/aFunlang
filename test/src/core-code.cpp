#include "code.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    {
        Code start = Code("test.aun");
        start.getByteCode()->connect(new Code::ByteCode(start, "Test", 1))->connect(
                new Code::ByteCode(start, Code::ByteCode::block_p, new Code::ByteCode(start, Code::ByteCode::block_p,
                                                                                      new Code::ByteCode(start, "Test3",
                                                                                                         2), 2), 2));
        start.display();
        std::string md5 = start.getMD5_v1();
        printf("md5: %s\n", md5.c_str());

        getEndian();

        start.writeByteCode("test.aun");
    }

    {
        Code start = Code("test.aun");
        start.readByteCode("test.aun");

        start.display();
        std::string md5 = start.getMD5_v1();
        printf("md5: %s\n", md5.c_str());
    }

    return 0;
}