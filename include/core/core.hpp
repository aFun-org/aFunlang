#ifndef AFUN_CORE_HPP
#define AFUN_CORE_HPP
#include "tool.hpp"

namespace aFuncore {
    typedef enum CodeType {
        code_start = 0,
        code_element = 1,
        code_block = 2,
    } CodeType;
    typedef enum BlockType {
        block_p = '(',
        block_b = '[',
        block_c = '{',
    } BlockType;
    typedef class Code Code;

    class EnvVarSpace;

    class Inter;
    enum InterStatus {
        inter_creat = 0,
        inter_init = 1,  // 执行初始化程序
        inter_normal = 2,  // 正常执行
        inter_stop = 3,  // 当前运算退出
        inter_exit = 4,  // 解释器退出
    };
    typedef enum InterStatus InterStatus;
    typedef enum ExitFlat {
        ef_activity = 0,  // 主动退出
        ef_passive = 1,  // 被动退出
        ef_none = 2,
    } ExitFlat;
    typedef enum ExitMode {
        em_activity = ef_activity,  // 主动退出
        em_passive = ef_passive,  // 被动退出
    } ExitMode;

    static const int PrefixCount = 2;
    typedef enum Prefix {
        prefix_quote = 0,  // 变量引用
        prefix_exec_first = 1,
    } Prefix;
    static const std::string E_PREFIX = "$`'";  /* NOLINT element前缀 */
    static const std::string B_PREFIX = "$`'%^&<?>";  /* NOLINT block前缀 */

    class Message;
    class NormalMessage;

    class MessageStream;
    class UpMessage;
    class DownMessage;

    class Activation;
    class ExeActivation;
    class TopActivation;
    class FuncActivation;
    typedef enum ActivationStatus {
        as_run = 0,
        as_end = 1,
    } ActivationStatus;

    class GcList;

    class Object;
    class Function;

    class Var;
    class VarSpace;
    class VarList;
    class ProtectVarSpace;
    typedef enum VarOperationFlat {
        vof_success = 0,  // 成功
        vof_not_var = 1,  // 变量不存在
        vof_redefine_var = 2,  // 变量重复定义
        vof_fail = 3,  // 存在其他错误
    } VarOperationFlat;
}

#endif //AFUN_CORE_HPP
