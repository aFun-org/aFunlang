#include "aFuntool.h"
#include <clocale>
using namespace aFuntool;

int main(int argc, char **argv) {
    wchar_t *tmp;
    const wchar_t *tmp2 = L"你好";
    const char *tmp3 = "你好";

    setlocale(LC_ALL, "");

#ifdef aFunWIN32_NO_CYGWIN
    convertWideByte(&tmp, tmp3, CP_UTF8);

    std::wcout << tmp << std::endl;
    std::wcout << tmp2 << std::endl;

    for (int i = 0; i < wcslen(tmp); i++)
        printf("%x ", tmp[i]);
    printf("\n");

    for (int i = 0; i < wcslen(tmp2); i++)
        printf("%x ", tmp2[i]);
    printf("\n");

    for (int i = 0; i < strlen(tmp3); i++)
        printf("%x ", (unsigned int)tmp3[i]);
    printf("\n");
#endif
    return 0;
}
