
/********************************************
mawk.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/*   $Log:	mawk.h,v $
 * Revision 3.7.1.1  91/09/14  17:23:46  brennan
 * VERSION 1.0
 * 
 * Revision 3.7  91/08/13  06:51:48  brennan
 * VERSION .9994
 * 
 * Revision 3.6  91/07/19  07:51:19  brennan
 * escape sequence now recognized in command line assignments
 * 
 * Revision 3.5  91/07/17  15:09:34  brennan
 * moved space_split() proto to field.h
 * 
 * Revision 3.4  91/06/29  09:47:18  brennan
 * Only track NR if needed
 * 
 * Revision 3.3  91/06/28  04:17:04  brennan
 * VERSION 0.999
 * 
 * Revision 3.2  91/06/10  15:59:23  brennan
 * changes for V7
 * 
 * Revision 3.1  91/06/07  10:27:54  brennan
 * VERSION 0.995
 * 
 * Revision 2.7  91/06/05  07:20:26  brennan
 * better error messages when regular expression compiles fail
 * 
 * Revision 2.6  91/06/04  06:45:58  brennan
 * minor change for includes
 * 
 * Revision 2.5  91/05/28  15:18:02  brennan
 * removed STRING_BUFF back to temp_buff.string_buff
 * 
 * Revision 2.4  91/05/28  09:04:59  brennan
 * removed main_buff
 * 
 * Revision 2.3  91/05/22  07:47:57  brennan
 * added strtod proto
 * 
 * Revision 2.2  91/05/16  12:20:00  brennan
 * cleanup of machine dependencies
 * 
 * Revision 2.1  91/04/08  08:23:33  brennan
 * VERSION 0.97
 * 
*/


/*  mawk.h  */

#ifndef  MAWK_H
#define  MAWK_H   

#include  "config.h"

#ifdef   DEBUG
#define  YYDEBUG  1
extern  int   yydebug ;  /* print parse if on */
extern  int   dump_RE ;
#endif
extern  int   dump_code ;

#include <stdio.h>

#if  HAVE_STRING_H
#include <string.h>
#else
char *strchr() ;
char *strcpy() ;
char *strrchr() ;
#endif

#if  HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "types.h"


/*----------------
 *  GLOBAL VARIABLES
 *----------------*/

/* some well known cells */
extern CELL cell_zero, cell_one ;
extern STRING  null_str ;
/* a useful scratch area */
extern union tbuff temp_buff ;

/* help with casts */
extern int pow2[] ;


 /* these are used by the parser, scanner and error messages
    from the compile  */

extern  int current_token ;
extern  unsigned  token_lineno ; /* lineno of current token */
extern  unsigned  compile_error_count ;
extern  int  paren_cnt, brace_cnt ;
extern  int  print_flag, getline_flag ;
extern  short mawk_state ;
#define EXECUTION 	1  /* other state is 0 compiling */

/*---------*/

#ifndef MSDOS_MSC
extern  int  errno ;     
#endif
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
char *PROTO( str_str, (char*, char*, unsigned) ) ;
char *PROTO( rm_escape, (char *) ) ;
int   PROTO( re_split, (char *, PTR) ) ;
char *PROTO( re_pos_match, (char *, PTR, unsigned *) ) ;

void  PROTO( exit, (int) ) ;
#ifdef THINK_C
#include <unix.h>
#else
int   PROTO( close, (int) ) ;
int   PROTO( open, (char *,int, int) ) ;
int   PROTO( read, (int , PTR, unsigned) ) ;
char *PROTO( getenv, (const char *) ) ;
#endif

int  PROTO ( parse, (void) ) ;
int  PROTO ( yylex, (void) ) ;
int  PROTO( yyparse, (void) ) ;
void PROTO( yyerror, (char *) ) ;

void PROTO( bozo, (char *) ) ;
void PROTO( errmsg , (int, char*, ...) ) ;
void PROTO( compile_error, ( char *, ...) ) ;

INST *PROTO( execute, (INST *, CELL *, CELL *) ) ;
char *PROTO( find_kw_str, (int) ) ;

#if ! HAVE_STDLIB_H
double strtod() ;
#endif

double fmod() ;

#endif  /* MAWK_H */
