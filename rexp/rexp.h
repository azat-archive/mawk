
/********************************************
rexp.h
copyright 1991, Michael D. Brennan

This is a source file for mawk an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*  rexp.h    */

#ifndef  REXP_H
#define  REXP_H

#include  <string.h>
#include  <stdio.h>
#include  <setjmp.h>

#ifndef   PROTO
#ifdef    __STDC__
#define  PROTO(name, args)   name  args
#else
#define  PROTO(name, args)   name()
#endif
#endif   

#ifdef  __STDC__
#define  VOID   void
#include <stdlib.h>
#else
#define  VOID   char
char *malloc(), *realloc() ;
void free() ;
#endif

/* user can change this  */

#define  RE_malloc(x)    RE_xmalloc(x)
#define  RE_realloc(x,l)   RE_xrealloc(x,l)
#define  RE_free(x)      free(x)

VOID  *PROTO( RE_xmalloc, (unsigned) ) ;
VOID  *PROTO( RE_xrealloc, (void *,unsigned) ) ;


/*  finite machine  state types  */

#define  M_STR     	0
#define  M_CLASS   	1
#define  M_ANY     	2
#define  M_START   	3
#define  M_END     	4
#define  M_U       	5
#define  M_1J      	6
#define  M_2JA     	7
#define  M_2JB     	8
#define  M_ACCEPT  	9
#define  U_ON      	10

#define  U_OFF     0
#define  END_OFF   0
#define  END_ON    (2*U_ON)


typedef  unsigned char BV[32] ;  /* bit vector */

typedef  struct
{ char type ;
  unsigned char  len ;  /* used for M_STR  */
  union
   { 
     char *str  ;  /* string */
     BV   *bvp ;   /*  class  */
     int   jump ;
   }  data ;
}     STATE  ;

#define  STATESZ  (sizeof(STATE))

typedef  struct
{ STATE  *start, *stop ; }   MACHINE ;


/*  tokens   */
#define  T_OR   1       /* | */
#define  T_CAT  2       
#define  T_STAR 3       /* * */
#define  T_PLUS 4       /* + */
#define  T_Q    5       /* ? */
#define  T_LP   6       /* ( */
#define  T_RP   7       /* ) */
#define  T_START 8      /* ^ */
#define  T_END  9       /* $ */
#define  T_ANY  10      /* . */
#define  T_CLASS 11     /* starts with [ */
#define  T_SLASH 12     /*  \  */
#define  T_CHAR  13     /* all the rest */
#define  T_STR   14
#define  T_U     15

/*  precedences and error codes  */
#define  L   0
#define  EQ  1
#define  G   2
#define  E1  (-1)
#define  E2  (-2)
#define  E3  (-3)
#define  E4  (-4)
#define  E5  (-5)
#define  E6  (-6)
#define  E7  (-7)

#define  MEMORY_FAILURE      5

/* struct for the run time stack */
typedef struct {
STATE *m ;   /*   save the machine ptr */
int    u ;   /*   save the u_flag */
char  *s ;   /*   save the active string ptr */
char  *ss ;  /*   save the match start -- only used by REmatch */
} RT_STATE ;   /* run time state */

/*  error  trap   */
extern int REerrno ;
MACHINE   PROTO(RE_error_trap, (int) ) ;


MACHINE   PROTO( RE_u, (void) ) ;
MACHINE   PROTO( RE_start, (void) ) ;
MACHINE   PROTO( RE_end, (void) ) ;
MACHINE   PROTO( RE_any, (void) ) ;
MACHINE   PROTO( RE_str, (char *, unsigned) ) ;
MACHINE   PROTO( RE_class, (BV *) ) ;
void      PROTO( RE_cat, (MACHINE *, MACHINE *) ) ;
void      PROTO( RE_or, (MACHINE *, MACHINE *) ) ;
void      PROTO( RE_close, (MACHINE *) ) ;
void      PROTO( RE_poscl, (MACHINE *) ) ;
void      PROTO( RE_01, (MACHINE *) ) ;
void      PROTO( RE_panic, (char *) ) ;
char     *PROTO( str_str, (char *, char *, unsigned) ) ;

void      PROTO( RE_lex_init , (char *) ) ;
int       PROTO( RE_lex , (MACHINE *) ) ;
void      PROTO( RE_run_stack_init, (void) ) ;
RT_STATE *PROTO( RE_new_run_stack, (void) ) ;

#endif   /* REXP_H  */
