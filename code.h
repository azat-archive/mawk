
/********************************************
code.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	code.h,v $
 * Revision 2.1  91/04/08  08:22:48  brennan
 * VERSION 0.97
 * 
*/


/*  code.h  */

#ifndef  CODE_H
#define  CODE_H

#include "memory.h"
#include <setjmp.h>

/* coding scope */
#define   SCOPE_MAIN    0
#define   SCOPE_BEGIN   1  
#define   SCOPE_END     2
#define   SCOPE_FUNCT   3


extern  INST  *code_ptr ;
extern  INST  *begin_start , *begin_code_ptr ;
extern  INST  *end_start , *end_code_ptr ;
extern  INST  *main_start, *main_code_ptr ;
extern  unsigned begin_size, end_size, main_size ;

extern  CELL  eval_stack[] ;


#define  code1(x)  code_ptr++ -> op = (x)

#define  code2(x,y)    (void)( code_ptr++ -> op = (x) ,\
                         code_ptr++ -> ptr = (PTR)(y) )


/*  the machine opcodes  */

#define _HALT            0
#define _STOP            1
#define _STOP0           2  
#define _PUSHC           3
#define _PUSHINT         4
#define _PUSHA           5
#define _PUSHI           6
#define L_PUSHA          7
#define L_PUSHI          8
#define AE_PUSHA         9
#define AE_PUSHI        10
#define A_PUSHA         11
#define LAE_PUSHA       12
#define LAE_PUSHI       13
#define LA_PUSHA        14
#define F_PUSHA         15
#define FE_PUSHA        16
#define F_PUSHI         17
#define FE_PUSHI        18
#define _POP            19
#define _PULL           20
#define _DUP            21
#define _ADD            22
#define _SUB            23
#define _MUL            24
#define _DIV            25
#define _MOD            26
#define _POW            27
#define _NOT            28
#define _TEST           29
#define A_TEST          30
#define A_DEL           31
#define A_LOOP          32
#define A_CAT           33
#define _UMINUS         34
#define _UPLUS          35
#define _ASSIGN         36
#define _ADD_ASG        37
#define _SUB_ASG        38
#define _MUL_ASG        39
#define _DIV_ASG        40
#define _MOD_ASG        41
#define _POW_ASG        42
#define F_ASSIGN        43
#define F_ADD_ASG       44
#define F_SUB_ASG       45
#define F_MUL_ASG       46
#define F_DIV_ASG       47
#define F_MOD_ASG       48
#define F_POW_ASG       49
#define _CAT            50
#define _BUILTIN        51
#define _PRINT          52
#define _POST_INC       53
#define _POST_DEC       54
#define _PRE_INC        55
#define _PRE_DEC        56
#define F_POST_INC      57
#define F_POST_DEC      58
#define F_PRE_INC       59
#define F_PRE_DEC       60
#define _JMP            61
#define _JNZ            62
#define _JZ             63
#define _EQ             64
#define _NEQ            65
#define _LT             66
#define _LTE            67
#define _GT             68
#define _GTE            69
#define _MATCH          70
#define _EXIT           71
#define _EXIT0          72
#define _NEXT           73
#define _RANGE          74
#define _CALL           75
#define _RET            76
#define _RET0           77


/* next and exit statements */

extern jmp_buf  exit_jump, next_jump ;
extern int exit_code ;

#endif  /* CODE_H */
