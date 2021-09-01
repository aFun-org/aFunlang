/*
 * 文件名: error_macro.h
 * 目标: 定义错误信息
 */

#ifndef AFUN_ERROR_MACRO_H
#define AFUN_ERROR_MACRO_H

#define SYNTAX_ERROR "Syntax-Error"
#define SYNTAX_ERROR_INFO "Block syntax errors." /* block元素不足 */

#define RUN_ERROR "Run-Error"
#define NOT_CODE_INFO "Not code to run."
#define FREE_VARSPACE_INFO "Free VarSpace error (VarSpace count error) ." /* 计数释放变量空间时产生错误 */
#define PURE_EMBEDDED_INFO "Super pure function and super embedded function not to be used together."
#define IMPLICIT_SET_INFO(name) "Implicit parameter (" #name ") cannot be set."
#define FUNCBODY_ERROR_INFO "Dynamic function body is not filled."  /* 执行函数体时, 获得func_body_dynamic类型的函数体 */
#define RETURN_OBJ_NOT_FOUND_INFO "Sequential execution but unable to return the first execution result (The result does not exist)."
#define NOT_MSG_INFO "Don't get msg after function call."

#define INFIX_PROTECT "Infix-Protect"
#define LITERAL_ERROR "Literal-Error"
#define VARIABLE_ERROR "Variable-Error"

#define CALL_ERRPR "Call-Error"
#define BRACKETS_FUNC_BODY_INFO "Brackets cannot get function body."
#define PARENTHESES_FUNC_BODY_INFO "Parentheses cannot get function body."
#define CURLY_FUNC_BODY_INFO "Curly cannot get function body."

#define TYPE_ERROR "Type=Error"
#define API_NOT_FOUND_INFO(name) ("Object API not found: " #name)

#define API_RUN_ERROR "API-Run-Error"
#define API_DONOT_GIVE(name) ("Object API don't give: " #name)

#endif //AFUN_ERROR_MACRO_H
