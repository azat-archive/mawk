
/********************************************
bi_funct.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	bi_funct.c,v $
 * Revision 2.3  91/04/17  06:34:00  brennan
 * index("","") should be 1 not 0 for consistency with match("",//)
 * 
 * Revision 2.2  91/04/09  12:38:42  brennan
 * added static to funct decls to satisfy STARDENT compiler
 * 
 * Revision 2.1  91/04/08  08:22:17  brennan
 * VERSION 0.97
 * 
*/


#include "mawk.h"
#include "bi_funct.h"
#include "bi_vars.h"
#include "memory.h"
#include "init.h"
#include "files.h"
#include "fin.h"
#include "field.h"
#include "regexp.h"
#include "repl.h"
#include <math.h>

#ifndef  BSD43
void PROTO( srand48, (long) ) ;
double PROTO( drand48, (void) ) ;
#endif

/* statics */
static STRING *PROTO(gsub, (PTR, CELL *, char *, int) ) ;
static void  PROTO( fplib_err, (char *, double, char *) ) ;


/* global for the disassembler */
BI_REC  bi_funct[] = { /* info to load builtins */

"index" , bi_index , 2, 2 ,
"substr" , bi_substr, 2, 3,
"sprintf" , bi_sprintf, 1, 255,
"sin", bi_sin , 1, 1 ,
"cos", bi_cos , 1, 1 ,
"atan2", bi_atan2, 2,2,
"exp", bi_exp, 1, 1,
"log", bi_log , 1, 1 ,
"int", bi_int, 1, 1,
"sqrt", bi_sqrt, 1, 1,
"rand" , bi_rand, 0, 0,
"srand", bi_srand, 0, 1,
"close", bi_close, 1, 1,
"system", bi_system, 1, 1,

#if  DOS   /* this might go away, when pipes and system are added
	      for DOS  */
"errmsg", bi_errmsg, 1, 1,
#endif

(char *) 0, (PF_CP) 0, 0, 0 } ;



void bi_funct_init()
{ register BI_REC *p = bi_funct ;
  register SYMTAB *stp ;

  while ( p->name )
  { stp = insert( p->name ) ;
    stp->type = ST_BUILTIN ;
    stp->stval.bip = p++ ;
  }
  /* seed rand() off the clock */
  { CELL c ;

    c.type = 0 ; (void) bi_srand(&c) ;
  }

  stp = insert( "length") ;
  stp->type = ST_LENGTH ;
}

/**************************************************
 string builtins (except split (in split.c) and [g]sub (at end))
 **************************************************/

CELL *bi_length(sp)
  register  CELL *sp ;
{ unsigned len ;

  if ( sp->type < C_STRING ) cast1_to_s(sp) ;
  len = string(sp)->len ;

  free_STRING( string(sp) ) ;
  sp->type = C_DOUBLE ;
  sp->dval = (double) len ;

  return sp ;
}

char *str_str(target, key , key_len)
  register char *target, *key ;
  unsigned key_len ;
{ 
  switch( key_len )
  { case 0 :  return (char *) 0 ;
    case 1 :  return strchr( target, *key) ;
    case 2 :
        while ( target = strchr(target, *key) )
          if ( target[1] == key[1] )  return  target ;
          else target++ ;
        /*failed*/
        return (char *) 0 ;
  }
  key_len-- ;
  while ( target = strchr(target, *key) )
        if ( memcmp(target+1, key+1, key_len) == 0 ) return target ;
        else target++ ;
  /*failed*/
  return (char *) 0 ;
}



CELL *bi_index(sp)
  register CELL *sp ;
{ register int idx ;
  unsigned len ;
  char *p ;

  sp-- ;
  if ( TEST2(sp) != TWO_STRINGS )
        cast2_to_s(sp) ;

  if ( len = string(sp+1)->len )
    idx = (p = str_str(string(sp)->str,string(sp+1)->str,len))
          ? p - string(sp)->str + 1 : 0 ;

  else  /* index of the empty string */
    idx = 1 ;
  
  free_STRING( string(sp) ) ;
  free_STRING( string(sp+1) ) ;
  sp->type = C_DOUBLE ;
  sp->dval = (double) idx ;
  return sp ;
}

/*  substr(s, i, n)
    if l = length(s)
    then get the characters
    from  max(1,i) to min(l,n-i-1) inclusive */

CELL *bi_substr(sp)
  CELL *sp ;
{ int n_args, len ;
  register int i, n ;
  char *s ;    /* substr(s, i, n) */
  STRING *sval ;

  n_args = sp->type ;
  sp -= n_args ;
  if ( sp->type < C_STRING )  cast1_to_s(sp) ;
  s = (sval = string(sp)) -> str ;

  if ( n_args == 2 )  
  { n = 0x7fff  ;  /* essentially infinity */
    if ( sp[1].type != C_DOUBLE ) cast1_to_d(sp+1) ; 
  }
  else
  { if ( sp[1].type + sp[2].type != TWO_STRINGS ) cast2_to_d(sp+1) ;
    n = (int) sp[2].dval ;
  }
  i = (int) sp[1].dval - 1 ; /* i now indexes into string */


  if ( (len = strlen(s)) == 0 )  return sp ;
  /* get to here is s is not the null string */
  if ( i < 0 ) { n += i ; i = 0 ; }
  if ( n > len - i )  n = len - i ;

  if ( n <= 0 )  /* the null string */
  { free_STRING( sval ) ;
    sp->ptr = (PTR) &null_str ;
    null_str.ref_cnt++ ;
  }
  else  /* got something */
  { 
    sp->ptr = (PTR) new_STRING((char *)0, n) ;
    (void) memcpy(string(sp)->str, s+i, n) ;
    string(sp)->str[n] = 0 ;
  }
  return sp ;
} 

/*
  match(s,r)
  sp[0] holds s, sp[-1] holds r
*/

CELL *bi_match(sp)
  register CELL *sp ;
{ double d ;
  char *p ;
  unsigned length ;

  if ( sp->type != C_RE )  cast_to_RE(sp) ;
  if ( (--sp)->type < C_STRING )  cast1_to_s(sp) ;

  if ( p = REmatch(string(sp)->str, (sp+1)->ptr, &length) )
      d = (double) ( p - string(sp)->str + 1 ) ;
  else  d = 0.0 ;

  cell_destroy( & bi_vars[RSTART] ) ;
  cell_destroy( & bi_vars[RLENGTH] ) ;
  bi_vars[RSTART].type = C_DOUBLE ;
  bi_vars[RSTART].dval = d ;
  bi_vars[RLENGTH].type = C_DOUBLE ;
  bi_vars[RLENGTH].dval = (double) length ;

  free_STRING(string(sp)) ;
    
  sp->type = C_DOUBLE ;  sp->dval = d ;
  return sp ;
}


/************************************************
  arithemetic builtins
 ************************************************/

static void fplib_err( fname, val, error)
  char *fname ;
  double val ;
  char *error ;
{
  rt_error("%s(%g) : %s" , fname, val, error) ;
}


CELL *bi_sin(sp)
  register CELL *sp ;
{ 
#if ! STDC_MATHERR
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  sp->dval = sin( sp->dval ) ;
  return sp ;
#else
  double x ;

  errno = 0 ;
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  x = sp->dval ;
  sp->dval = sin( sp->dval ) ;
  if ( errno )  fplib_err("sin", x, "loss of precision") ;
  return sp ;
#endif
}

CELL *bi_cos(sp)
  register CELL *sp ;
{ 
#if ! STDC_MATHERR
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  sp->dval = cos( sp->dval ) ;
  return sp ;
#else
  double x ;

  errno = 0 ;
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  x = sp->dval ;
  sp->dval = cos( sp->dval ) ;
  if ( errno )  fplib_err("cos", x, "loss of precision") ;
  return sp ;
#endif
}

CELL *bi_atan2(sp)
  register CELL *sp ;
{ 
#if  !  STDC_MATHERR
  sp-- ;
  if ( TEST2(sp) != TWO_DOUBLES ) cast2_to_d(sp) ;
  sp->dval = atan2(sp->dval, (sp+1)->dval) ;
  return sp ;
#else

  errno = 0 ;
  sp-- ;
  if ( TEST2(sp) != TWO_DOUBLES ) cast2_to_d(sp) ;
  sp->dval = atan2(sp->dval, (sp+1)->dval) ;
  if ( errno ) rt_error("atan2(0,0) : domain error") ;
  return sp ;
#endif
}

CELL *bi_log(sp)
  register CELL *sp ;
{ 
#if ! STDC_MATHERR
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  sp->dval = log( sp->dval ) ;
  return sp ;
#else
  double  x ;

  errno = 0 ;
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  x = sp->dval ;
  sp->dval = log( sp->dval ) ;
  if ( errno )  fplib_err("log", x, "domain error") ;
  return sp ;
#endif
}

CELL *bi_exp(sp)
  register CELL *sp ;
{ 
#if  ! STDC_MATHERR
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  sp->dval = exp(sp->dval) ;
  return sp ;
#else
  double  x ;

  errno = 0 ;
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  x = sp->dval ;
  sp->dval = exp(sp->dval) ;
  if ( errno && sp->dval)  fplib_err("exp", x, "overflow") ;
     /* on underflow sp->dval==0, ignore */
  return sp ;
#endif
}

CELL *bi_int(sp)
  register CELL *sp ;
{ if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  sp->dval = sp->dval >= 0.0 ? floor( sp->dval ) : ceil(sp->dval)  ;
  return sp ;
}

CELL *bi_sqrt(sp)
  register CELL *sp ;
{ 
#if  ! STDC_MATHERR
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  sp->dval = sqrt( sp->dval ) ;
  return sp ;
#else
  double x ;

  errno = 0 ;
  if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
  x = sp->dval ;
  sp->dval = sqrt( sp->dval ) ;
  if ( errno )  fplib_err("sqrt", x, "domain error") ;
  return sp ;
#endif
}

#ifdef  __TURBOC__
long  biostime(int, long) ;
#define  time(x)  (biostime(0,0L)<<4)
#else
#include <sys/types.h>

#if 0
#ifndef  STARDENT
#include <sys/timeb.h>
#endif
#endif

#endif

CELL *bi_srand(sp)
  register CELL *sp ;
{ register long l ; 
  void srand48() ;

  if ( sp-- -> type )  /* user seed */
  { if ( sp->type != C_DOUBLE )  cast1_to_d(sp) ;
    l = (long) sp->dval ; }
  else
  { l = (long) time( (time_t *) 0 ) ;
    (++sp)->type = C_DOUBLE ;
    sp->dval = (double) l ;
  }
  srand48(l) ;
  return sp ;
}
    
CELL *bi_rand(sp)
  register CELL *sp ;
{ 

  (++sp)->type = C_DOUBLE ;
  sp->dval = drand48() ;
  return sp ;
}

/*************************************************
 miscellaneous builtins
 close, system and getline
 *************************************************/

CELL *bi_close(sp)
  register CELL *sp ;
{ int x ;

  if ( sp->type < C_STRING ) cast1_to_s(sp) ;
  x = file_close( (STRING *) sp->ptr) ;
  free_STRING( string(sp) ) ;
  sp->type = C_DOUBLE ;
  sp->dval = (double) x ;
  return sp ;
}

#if   ! DOS
CELL *bi_system(sp)
  CELL *sp ;
{ int pid ;
  unsigned ret_val ;

  if ( !shell ) shell = (shell = getenv("SHELL")) ? shell : "/bin/sh" ;
  if ( sp->type < C_STRING ) cast1_to_s(sp) ;

  switch( pid = fork() )
  { case -1 :  /* fork failed */

       errmsg(errno, "could not create a new process") ;
       ret_val = 128 ;
       break ;

    case  0  :  /* the child */
       (void) execl(shell, shell, "-c", string(sp)->str, (char *) 0) ;
       /* if get here, execl() failed */
       errmsg(errno, "execute of %s failed", shell) ;
       fflush(stderr) ;
       _exit(128) ;

    default   :  /* wait for the child */
       ret_val = wait_for(pid) ;
       if ( ret_val & 0xff ) ret_val = 128 ;
       else  ret_val = (ret_val & 0xff00) >> 8 ;
       break ;
  }

  cell_destroy(sp) ;
  sp->type = C_DOUBLE ;
  sp->dval = (double) ret_val ;
  return sp ;
}

#else   /*  DOS   */

CELL *bi_system( sp )
  register CELL *sp ;
{ rt_error("no system call in MsDos --yet") ;
  return sp ;
}

/* prints errmsgs for DOS  */
CELL *bi_errmsg(sp)
  register CELL *sp ;
{
  cast1_to_s(sp) ;
  fprintf(stderr, "%s\n", string(sp)->str) ;
  free_STRING(string(sp)) ;
  sp->type = C_DOUBLE ;
  sp->dval = 0.0 ;
  return sp ;
}
#endif


/*  getline()  */

/*  if type == 0 :  stack is 0 , target address

    if type == F_IN : stack is F_IN, expr(filename), target address

    if type == PIPE_IN : stack is PIPE_IN, target address, expr(pipename)
*/

CELL *bi_getline(sp)
  register CELL *sp ;
{ 
  CELL tc , *cp ;
  char *p ;
  unsigned len ;
  FIN *fin_p ;


  switch( sp->type )
  { 
    case 0 :
        sp-- ;
        if ( main_fin == (FIN *) -1 && ! open_main() )
                goto open_failure ;
    
        if ( ! main_fin || !(p = FINgets(main_fin, &len)) )
                goto  eof ;

        cp = (CELL *) sp->ptr ;
        if ( TEST2(bi_vars+NR) != TWO_DOUBLES ) cast2_to_d(bi_vars+NR) ;
        bi_vars[NR].dval += 1.0 ;
        bi_vars[FNR].dval += 1.0 ;
        break ;

    case  F_IN :
        sp-- ;
        if ( sp->type < C_STRING ) cast1_to_s(sp) ;
        fin_p = (FIN *) file_find(sp->ptr, F_IN) ;
        free_STRING(string(sp) ) ;
        sp-- ;

        if ( ! fin_p )   goto open_failure ;
        if ( ! (p = FINgets(fin_p, &len)) )  goto eof ; 
        cp = (CELL *) sp->ptr ;
        break ;

    case PIPE_IN :
        sp -= 2 ;
        if ( sp->type < C_STRING ) cast1_to_s(sp) ;
        fin_p = (FIN *) file_find(sp->ptr, PIPE_IN) ;
        free_STRING(string(sp)) ;

        if ( ! fin_p )   goto open_failure ;
        if ( ! (p = FINgets(fin_p, &len)) )  goto eof ; 
        cp = (CELL *) (sp+1)->ptr ;
        break ;

    default : bozo("type in bi_getline") ;

  }

  /* we've read a line , store it */

    if ( len == 0 )
    { tc.type = C_STRING ; 
      tc.ptr = (PTR) &null_str ; 
      null_str.ref_cnt++ ;
    }
    else
    { tc.type = C_MBSTRN ;
      tc.ptr = (PTR) new_STRING((char *) 0, len) ;
      (void) memcpy( string(&tc)->str, p, len) ;
    }

    if ( cp  >= field && cp < field+NUM_FIELDS )
           field_assign(cp-field, &tc) ;
    else
    { cell_destroy(cp) ;
      (void) cellcpy(cp, &tc) ;
    }

    cell_destroy(&tc) ;

  sp->dval = 1.0  ;  goto done ;

open_failure :
  sp->dval = -1.0  ; goto done ;

eof :
  sp->dval = 0.0  ;  /* fall thru to done  */

done :
  sp->type = C_DOUBLE  ;
  return sp ;
}

/**********************************************
 sub() and gsub()
 **********************************************/

/* entry:  sp[0] = address of CELL to sub on
           sp[-1] = substitution CELL
           sp[-2] = regular expression to match
*/

CELL *bi_sub( sp )
  register CELL *sp ;
{ CELL *cp ; /* pointer to the replacement target */
  CELL tc ;  /* build the new string here */
  CELL sc ;  /* copy of the target CELL */
  char *front, *middle, *back ; /* pieces */
  unsigned front_len, middle_len, back_len ;

  sp -= 2 ;
  if ( sp->type != C_RE )  cast_to_RE(sp) ;
  if ( sp[1].type != C_REPL && sp[1].type != C_REPLV )
              cast_to_REPL(sp+1) ;
  cp = (CELL *) (sp+2)->ptr ;
  /* make a copy of the target, because we won't change anything
     including type unless the match works */
  (void) cellcpy(&sc, cp) ;
  if ( sc.type < C_STRING ) cast1_to_s(&sc) ;
  front = string(&sc)->str ;

  if ( middle = REmatch(front, sp->ptr, &middle_len) )
  { 
    front_len = middle - front ;
    back = middle + middle_len ; 
    back_len = string(&sc)->len - front_len - middle_len ;

    if ( (sp+1)->type == C_REPLV ) 
    { STRING *sval = new_STRING((char *) 0, middle_len) ;

      (void) memcpy(sval->str, middle, middle_len) ;
      (void) replv_to_repl(sp+1, sval) ;
      free_STRING(sval) ;
    }

    tc.type = C_STRING ;
    tc.ptr = (PTR) new_STRING((char *) 0, 
             front_len + string(sp+1)->len + back_len ) ;

    { char *p = string(&tc)->str ;

      if ( front_len )
      { (void) memcpy(p, front, front_len) ;
        p += front_len ;
      }
      if ( string(sp+1)->len )
      { (void) memcpy(p, string(sp+1)->str, string(sp+1)->len) ;
        p += string(sp+1)->len ;
      }
      if ( back_len )  (void) memcpy(p, back, back_len) ;
    }

    if ( cp  >= field && cp < field+NUM_FIELDS )
           field_assign(cp-field, &tc) ;
    else
    { cell_destroy(cp) ;
      (void) cellcpy(cp, &tc) ;
    }

    free_STRING(string(&tc)) ;
  }

  free_STRING(string(&sc)) ;
  repl_destroy(sp+1) ;
  sp->type = C_DOUBLE ;
  sp->dval = middle != (char *) 0 ? 1.0 : 0.0 ;
  return sp ;
}

static  unsigned repl_cnt ;  /* number of global replacements */

/* recursive global subsitution 
   dealing with empty matches makes this mildly painful
*/

static STRING *gsub( re, repl, target, flag)
  PTR  re ;
  CELL *repl ;  /* always of type REPL or REPLV */
  char *target ;
  int flag ; /* if on, match of empty string at front is OK */
{ char *front, *middle ;
  STRING *back ;
  unsigned front_len, middle_len ;
  STRING  *ret_val ;
  CELL xrepl ; /* a copy of repl so we can change repl */

  if ( ! (middle = REmatch(target, re, &middle_len)) )
      return  new_STRING(target) ; /* no match */

  (void) cellcpy(&xrepl, repl) ;

  if ( !flag && middle_len == 0 && middle == target ) 
  { /* match at front that's not allowed */

    if ( *target == 0 )  /* target is empty string */
    { null_str.ref_cnt++ ;
      return & null_str ;
    }
    else
    { char xbuff[2] ;

      front_len = 0 ;
      /* make new repl with target[0] */
      repl_destroy(repl) ;
      xbuff[0] = *target++ ;  xbuff[1] = 0 ;
      repl->type = C_REPL ;
      repl->ptr = (PTR) new_STRING( xbuff ) ;
      back = gsub(re, &xrepl, target, 1) ;
    }
  }
  else  /* a match that counts */
  { repl_cnt++ ;

    front = target ;
    front_len = middle - target ;

    if ( *middle == 0 )  /* matched back of target */
    { back = &null_str ; null_str.ref_cnt++ ; }
    else back = gsub(re, &xrepl, middle + middle_len, 0) ;
      
    /* patch the &'s if needed */
    if ( repl->type == C_REPLV )
    { STRING *sval = new_STRING((char *) 0, middle_len) ;

      (void) memcpy(sval->str, middle, middle_len) ;
      (void) replv_to_repl(repl, sval) ;
      free_STRING(sval) ;
    }
  }

  /* put the three pieces together */
  ret_val = new_STRING((char *)0,
              front_len + string(repl)->len + back->len); 
  { char *p = ret_val->str ;

    if ( front_len )
    { (void) memcpy(p, front, front_len) ; p += front_len ; }
    if ( string(repl)->len )
    { (void) memcpy(p, string(repl)->str, string(repl)->len) ;
      p += string(repl)->len ;
    }
    if ( back->len ) (void) memcpy(p, back->str, back->len) ;
  }

  /* cleanup, repl is freed by the caller */
  repl_destroy(&xrepl) ;
  free_STRING(back) ;

  return ret_val ;
}

/* set up for call to gsub() */
CELL *bi_gsub( sp )
  register CELL *sp ;
{ CELL *cp ;  /* pts at the replacement target */
  CELL sc  ;  /* copy of replacement target */
  CELL tc  ;  /* build the result here */

  sp -= 2 ;
  if ( sp->type != C_RE ) cast_to_RE(sp) ;
  if ( (sp+1)->type != C_REPL && (sp+1)->type != C_REPLV )
          cast_to_REPL(sp+1) ;

  (void) cellcpy(&sc, cp = (CELL *)(sp+2)->ptr) ;
  if ( sc.type < C_STRING ) cast1_to_s(&sc) ;

  repl_cnt = 0 ;
  tc.ptr = (PTR) gsub(sp->ptr, sp+1, string(&sc)->str, 1) ;

  if ( repl_cnt )
  { tc.type = C_STRING ;

    if ( cp >= field && cp < field + NUM_FIELDS )
        field_assign(cp-field, &tc) ;
    else
    { cell_destroy(cp) ; (void) cellcpy(cp, &tc) ; }
  }

  /* cleanup */
  free_STRING(string(&sc)) ; free_STRING(string(&tc)) ;
  repl_destroy(sp+1) ;

  sp->type = C_DOUBLE ;
  sp->dval = (double) repl_cnt ;
  return sp ;
}
