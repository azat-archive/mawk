
/********************************************
mawk.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/*   $Log:	mawk.h,v $
 * Revision 2.1  91/04/08  08:23:33  brennan
 * VERSION 0.97
 * 
*/


/*  mawk.h  */

#ifndef  MAWK_H
#define  MAWK_H   

#include  "machine.h"

#ifdef   DEBUG
#define  YYDEBUG  1
extern  int   yydebug ;  /* print parse if on */
extern  int   dump_RE ;
#endif
extern  int   dump_code ;

#ifdef  __STDC__
#define  PROTO(name, args)   name  args
#undef   HAVE_VOID_PTR
#define  HAVE_VOID_PTR          1
#else
#define  PROTO(name, args)   name()
#endif 


#include <stdio.h>
#include <string.h>
#include "types.h"


/*----------------
 *  GLOBAL VARIABLES
 *----------------*/

/* some well known cells */
extern CELL cell_zero, cell_one ;
extern STRING  null_str ;
/* a useful scratch area */
extern union tbuff temp_buff ;
extern char *main_buff ; /* main file input buffer */

/* help with casts */
extern int pow2[] ;


 /* these are used by the parser, scanner and error messages
    from the compile  */

extern  int current_token ;
extern  unsigned  token_lineno ; /* lineno of current token */
extern  unsigned  compile_error_count ;
extern  int  paren_cnt, brace_cnt ;
extern  int  print_flag, getline_flag ;


/*---------*/

extern  int  errno ;     
extern  char *progname ; /* for error messages */

/* macro to test the type of two adjacent cells */
#define TEST2(cp)  (pow2[(cp)->type]+pow2[((cp)+1)->type])

/* macro to get at the string part of a CELL */
#define string(cp) ((STRING *)(cp)->ptr)

#ifdef   DEBUG
#define cell_destroy(cp)  DB_cell_destroy(cp)
#else

#define cell_destroy(cp)   if ( (cp)->type >= C_STRING &&\
                                -- string(cp)->ref_cnt == 0 )\
                                zfree(string(cp),string(cp)->len+5);else
#endif

/*  prototypes  */

void  PROTO( cast1_to_s, (CELL *) ) ;
void  PROTO( cast1_to_d, (CELL *) ) ;
void  PROTO( cast2_to_s, (CELL *) ) ;
void  PROTO( cast2_to_d, (CELL *) ) ;
void  PROTO( cast_to_RE, (CELL *) ) ;
void  PROTO( cast_for_split, (CELL *) ) ;
void  PROTO( check_strnum, (CELL *) ) ;
void  PROTO( cast_to_REPL, (CELL *) ) ;

int   PROTO( test, (CELL *) ) ; /* test for null non-null */
CELL *PROTO( cellcpy, (CELL *, CELL *) ) ;
CELL *PROTO( repl_cpy, (CELL *, CELL *) ) ;
void  PROTO( DB_cell_destroy, (CELL *) ) ;
void  PROTO( overflow, (char *, unsigned) ) ;
void  PROTO( rt_overflow, (char *, unsigned) ) ;
void  PROTO( rt_error, ( char *, ...) ) ;
void  PROTO( mawk_exit, (int) ) ;
void PROTO( da, (INST *, FILE *)) ;
int  PROTO( space_split, (char *) ) ;
char *PROTO( str_str, (char*, char*, unsigned) ) ;
int   PROTO( re_split, (char *, PTR) ) ;
char *PROTO( re_pos_match, (char *, PTR, unsigned *) ) ;

void  PROTO( exit, (int) ) ;
int   PROTO( close, (int) ) ;
int   PROTO( open, (char *,int, int) ) ;
int   PROTO( read, (int , PTR, unsigned) ) ;
char *PROTO( getenv, (const char *) ) ;

int  PROTO ( parse, (void) ) ;
int  PROTO ( yylex, (void) ) ;
int  PROTO( yyparse, (void) ) ;
void PROTO( yyerror, (char *) ) ;

void PROTO( bozo, (char *) ) ;
void PROTO( errmsg , (int, char*, ...) ) ;
void PROTO( compile_error, ( char *, ...) ) ;

INST *PROTO( execute, (INST *, CELL *, CELL *) ) ;
char *PROTO( find_kw_str, (int) ) ;

double strtod(), fmod() ;

#endif  /* MAWK_H */
