HELP_INFO = r'''[参数]
   help
       -v 显示版本信息
       -h 显示帮助信息
   run
       第一个参数为运行文件
       其后参数为命令行参数
   cl
       -e/--eval    运行指定代码(来自参数)
       -f/--file    运行文件 (.aun or .aub)
       -s/--source  运行源代码
       -b/--byte    运行字节码
       --no-aub     不保存字节码
       --no-cl      不进入命令行模式
       -- 前的自由参数被解析为-f参数
       -- 后的自由参数被解析为命令行参数
   build
       -o 文件输出位置(完整目录, 仅支持单个文件编译)
       -p 文件输出位置(文件夹, 支持多个文件编译)
       -f 强制编译

[Github page]
   <https://github.com/aFun-org/aFunlang>'''