#include "reader.h"

class ReaderText : public aFuncore::Reader {
    int count;
public:
    ReaderText() : Reader{"Test", 0} {
        count = 0;
    }

    size_t readText(char *dest, size_t len, ReadMode &mode) override {
        count++;
        if (count == 1) {
            memset(dest, 's', len);
            return len;
        } else if (count == 2) {
            memset(dest, 'w', len - 5);
            return len - 5;
        }
        mode = read_mode_finished;
        return 0;
    }
};

int main() {
    printf("HelloWorld\n");

    {
        ReaderText test = ReaderText();
        test.readFirstWord();

        char ch;
        size_t count = 0;
        do {
            ch = test.getChar();
            printf("%zu\t\tch = %c\n", count, ch);
            count++;
        } while (ch != aFuntool::NUL);

    }

    {
        ReaderText test = ReaderText();
        test.readFirstWord();

        char ch;
        for (size_t i = 0; i < 1000; i++) {
            ch = test.getChar();
            printf("%zu\t\tch = %c\n", i, ch);
        }
        char *new_word = test.readWord(100);  // 提取前面100个值
        aFuntool::safeFree(new_word);

        size_t count = 0;
        do {
            ch = test.getChar();
            printf("%zu\t\tch = %c\n", count, ch);  // 923-s 1019-w
            count++;
        } while (ch != aFuntool::NUL);
    }

    return 0;
}