
/********************************************
bi_funct.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	bi_funct.h,v $
 * Revision 2.2  91/04/22  08:00:13  brennan
 * prototype for bi_errmsg() under DOS
 * 
 * Revision 2.1  91/04/08  08:22:20  brennan
 * VERSION 0.97
 * 
*/

#ifndef  BI_FUNCT_H
#define  BI_FUNCT_H  1

#include "symtype.h"

extern BI_REC  bi_funct[] ;

void PROTO(bi_init, (void) ) ;

/* builtin string functions */
CELL *PROTO( bi_print, (CELL *) ) ;
CELL *PROTO( bi_printf, (CELL *) ) ;
CELL *PROTO( bi_length, (CELL *) ) ;
CELL *PROTO( bi_index, (CELL *) ) ;
CELL *PROTO( bi_substr, (CELL *) ) ;
CELL *PROTO( bi_sprintf, (CELL *) ) ;
CELL *PROTO( bi_split, (CELL *) ) ;
CELL *PROTO( bi_match, (CELL *) ) ;
CELL *PROTO( bi_getline, (CELL *) ) ;
CELL *PROTO( bi_sub, (CELL *) ) ;
CELL *PROTO( bi_gsub, (CELL *) ) ;

/* builtin arith functions */
CELL *PROTO( bi_sin, (CELL *) ) ;
CELL *PROTO( bi_cos, (CELL *) ) ;
CELL *PROTO( bi_atan2, (CELL *) ) ;
CELL *PROTO( bi_log, (CELL *) ) ;
CELL *PROTO( bi_exp, (CELL *) ) ;
CELL *PROTO( bi_int, (CELL *) ) ;
CELL *PROTO( bi_sqrt, (CELL *) ) ;
CELL *PROTO( bi_srand, (CELL *) ) ;
CELL *PROTO( bi_rand, (CELL *) ) ;

/* other builtins */
CELL *PROTO( bi_close, (CELL *) ) ;
CELL *PROTO( bi_system, (CELL *) ) ;

#if  DOS
CELL *PROTO(bi_errmsg, (CELL *) ) ;
#endif

#endif  /* BI_FUNCT_H  */

