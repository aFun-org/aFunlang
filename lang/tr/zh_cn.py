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
   
VERSION_N = '版本'
CL_N = '命令行'
CL_TIPS = '在顶层活动空间运行输入的代码'
USAGE_N = '帮助'
CL_ERROR = '命令行参数错误'
NOT_FILE = '没有代码需要被运行'
RUN_Exit_N = '退出'
RUN_Exitcode_N = '退出代码'
ARG_CONFLICT = '参数冲突'
NOT_BUILD_SRC = '没有源码被编译'
MANY_BUILD_SRC = '太多源码被编译'
NOT_BUILD_FILE_EXISITS = '需要编译的文件不存在'
ALREADY_BUILD = '已经编译'
BUILD_AGAIN = '使用 --fource 强制编译'
BUILD_FILE = '文件将被编译'
RUN_SAVE_ERROR = '字节码编译失败'
RUN_NOT_AUN = '运行的文件不是.aun文件'
RUN_NOT_AUB = '运行的文件不是.aub文件'
LOAD_BT_ERROR = '字节码加载失败'
RUN_NOT_AUN_AUB = '运行的文件不是.aun或.aub文件'
NOT_RUN_FILE_EXISITS = '需要运行的文件不存在'
BUILD_IN_AUN = '被编译的文件不是.aun文件'
BUILD_OUT_AUB = '编译输出的文件不是.aun文件'
BUILD_ERROR_N = '编译失败'