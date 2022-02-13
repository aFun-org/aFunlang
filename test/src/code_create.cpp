#include "aFuncode.h"

int main() {
    {
        auto start = aFuncode::Code("test.aun");
        start.getByteCode()->connect(new aFuncode::Code::ByteCode(start, "Test", 1))->connect(
                new aFuncode::Code::ByteCode(start,
                                             aFuncode::Code::ByteCode::block_p,
                                             new aFuncode::Code::ByteCode(start,
                                                                          aFuncode::Code::ByteCode::block_p,
                                                                          new aFuncode::Code::ByteCode(start,
                                                                                                       "Test3",
                                                                                                       2), 2), 2));
        start.display();
        std::string md5 = start.getMD5_v1();
        printf("md5: %s\n", md5.c_str());

        aFuntool::getEndian();

        start.writeByteCode("test.aun");
    }

    {
        aFuncode::Code start = aFuncode::Code("test.aun");
        start.readByteCode("test.aun");

        start.display();
        std::string md5 = start.getMD5_v1();
        printf("md5: %s\n", md5.c_str());
    }

    return 0;
}