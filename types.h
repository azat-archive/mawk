
/********************************************
types.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	types.h,v $
 * Revision 2.1  91/04/08  08:24:15  brennan
 * VERSION 0.97
 * 
*/


/*  types.h  */

#ifndef  TYPES_H
#define  TYPES_H

#if     HAVE_VOID_PTR
typedef  void *PTR ;
#else
typedef  char *PTR ;
#endif

#include  "sizes.h"


/*  CELL  types  */

#define  C_NOINIT                0
#define  C_DOUBLE                1
#define  C_STRING                2
#define  C_STRNUM                3
#define  C_MBSTRN                4 
        /*could be STRNUM, has not been checked */
#define  C_RE                    5
#define  C_SPACE                 6
        /* split on space */
#define  C_SNULL                 7
        /* split on the empty string  */
#define  C_REPL                  8
        /* a replacement string   '\&' changed to &  */
#define  C_REPLV                 9
        /* a vector replacement -- broken on &  */
#define  NUM_CELL_TYPES         10

/* these defines are used to check types for two
   CELLs which are adjacent in memory */

#define  TWO_NOINITS  (2*(1<<C_NOINIT))
#define  TWO_DOUBLES  (2*(1<<C_DOUBLE))
#define  TWO_STRINGS  (2*(1<<C_STRING))
#define  TWO_STRNUMS  (2*(1<<C_STRNUM))
#define  TWO_MBSTRNS  (2*(1<<C_MBSTRN))
#define  NOINIT_AND_DOUBLE  ((1<<C_NOINIT)+(1<<C_DOUBLE))
#define  NOINIT_AND_STRING  ((1<<C_NOINIT)+(1<<C_STRING))
#define  NOINIT_AND_STRNUM  ((1<<C_NOINIT)+(1<<C_STRNUM))
#define  DOUBLE_AND_STRING  ((1<<C_DOUBLE)+(1<<C_STRING))
#define  DOUBLE_AND_STRNUM  ((1<<C_STRNUM)+(1<<C_DOUBLE))
#define  STRING_AND_STRNUM  ((1<<C_STRING)+(1<<C_STRNUM))
#define  NOINIT_AND_MBSTRN  ((1<<C_NOINIT)+(1<<C_MBSTRN))
#define  DOUBLE_AND_MBSTRN  ((1<<C_DOUBLE)+(1<<C_MBSTRN))
#define  STRING_AND_MBSTRN  ((1<<C_STRING)+(1<<C_MBSTRN))
#define  STRNUM_AND_MBSTRN  ((1<<C_STRNUM)+(1<<C_MBSTRN))

typedef  struct {
unsigned short ref_cnt ;
unsigned short len ;
char str[4] ;
} STRING ;


typedef  struct cell {
short type ;
short vcnt ; /* only used if type == C_REPLV   */
PTR   ptr ;
double  dval ;
}  CELL ;


/* all builtins are passed the evaluation stack pointer and
   return its new value, here is the type */

#ifdef __STDC__
typedef CELL *(*PF_CP)(CELL *) ;
#else
typedef CELL *(*PF_CP)() ;
#endif

/* an element of code (instruction) */
typedef  union {
int  op ;
PTR  ptr ;
}  INST ;

/* a scratch buffer type */
union tbuff {
PTR   ptr_buff[MAX_FIELD] ;
char   string_buff[TEMP_BUFF_SZ + BUFFSZ + 1] ;
} ;

#endif
