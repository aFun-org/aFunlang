help_info = r'''[参数]
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
       -o/--out 文件输出位置(完整目录, 仅支持单个文件编译)
       -p/--path 文件输出位置(文件夹, 支持多个文件编译)
       -f/--fource 强制编译

[Github page]
   <https://github.com/aFun-org/aFunlang>'''
   
command_line_n = '命令行'
command_line_tips = '在顶层活动空间运行输入的代码'
usage_n = '帮助'
cl_arg_error_e = '命令行参数错误'
cl_not_file_e = '没有代码需要被运行'
exit_code_n = '退出代码'
arg_conflict_n = '参数冲突'
build_not_src_e = '没有源码被编译'
build_many_src_e = '太多源码被编译 (不要使用 --out 参数)'
build_src_not_exists_e = '需要编译的文件不存在'
build_src_already = '已经编译'
build_use_f = '使用 --fource 强制编译'
build_file = '文件将被编译'
run_save_e = '字节码编译失败'
run_source_not_aub_e = '运行的文件不是.aun文件'
run_bt_not_aub_e = '运行的文件不是.aub文件'
run_load_bt_e = '字节码加载失败'
run_file_aun_aub_e = '运行的文件不是.aun或.aub文件'
run_file_not_exists_e = '需要运行的文件不存在'
build_in_aun_e = '被编译的文件不是.aun文件'
build_out_aub_e = '编译输出的文件不是.aun文件'
build_error_e = '编译失败'
Interrupt_n = '中断'