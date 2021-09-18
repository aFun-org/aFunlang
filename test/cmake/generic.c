// 测试 generic 编译

int main() {int a; _Generic((a), int : 0, default : 0); return 0;}