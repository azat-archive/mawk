
/********************************************
split.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	split.c,v $
 * Revision 2.1  91/04/08  08:24:11  brennan
 * VERSION 0.97
 * 
*/

/* split.c */

#include "mawk.h"
#include "symtype.h"
#include "bi_vars.h"
#include "bi_funct.h"
#include "memory.h"
#include "scan.h"
#include "regexp.h"
#include "field.h"
#include <string.h>


/* split string s on SPACE without changing s.
   load the pieces into STRINGS and ptrs into
   temp_buff.ptr_buff[] 
   return the number of pieces */

int space_split( s )  
  register char *s ;
{ char *back = strchr(s,0) ;
  int i = 0 ;
  int len ;
  char *q ;
  STRING  *sval ;

  while ( 1 )
  { while ( scan_code[*(unsigned char*)s] == SC_SPACE )  s++ ;
    if ( *s == 0 )  break ;
    /* mark the front with q */
    q = s++ ;
    *back = ' ' ; /* sentinal */
    while ( scan_code[*(unsigned char*)s] != SC_SPACE )  s++ ;
    *back = 0 ;
    sval = (STRING *) (temp_buff.ptr_buff[i++] = 
         (PTR) new_STRING((char *) 0, len = s - q )) ;
    (void) memcpy(sval->str, q, len) ;
  }
  if ( i > MAX_FIELD ) 
     rt_overflow("maximum number of fields", MAX_FIELD) ;
  return i ;
}


char *re_pos_match(s, re, lenp)
  register char *s ; 
  PTR re ; unsigned *lenp ;
{
  while ( s = REmatch(s, re, lenp) )
        if ( *lenp )   return s ;
        else
        if ( *s == 0 )  break ;
        else s++ ;

  return (char *) 0 ;
}

int re_split(s, re)
  char *s ;
  PTR  re ;
{ register char *t ;
  int i = 0 ;
  unsigned mlen, len ;
  STRING *sval ;

  while ( t = re_pos_match(s, re, &mlen) )
  { sval = (STRING*)(temp_buff.ptr_buff[i++] = (PTR)
            new_STRING( (char *)0, len = t-s) ) ;
    (void) memcpy(sval->str, s, len) ;
    s = t + mlen ;
  }
  temp_buff.ptr_buff[i++] = (PTR) new_STRING(s) ;
  if ( i > MAX_FIELD ) 
     rt_overflow("maximum number of fields", MAX_FIELD) ;
  return i ;
}
    
/*  split(s, X, r)
    split s into array X on r

    entry: sp[0] holds r
           sp[-1] pts at X
           sp[-2] holds s
*/
CELL *bi_split(sp)
  register CELL *sp ;
{ 
  int cnt ;   /* the number of pieces */
  double dcnt ; /* double version of cnt */
  ARRAY A ;
  CELL  *cp ;
  char *ofmt ;


  if ( sp->type < C_RE )  cast_for_split(sp) ;
        /* can be C_RE, C_SPACE or C_SNULL */
  sp -= 2 ;
  if ( sp->type < C_STRING )  cast1_to_s(sp) ;

  if ( string(sp)->len == 0 ) /* nothing to split */
  { free_STRING( string(sp) ) ;
    sp->type = C_DOUBLE ; sp->dval = 0.0 ;
    return sp ;
  }

  switch ( (sp+2)->type )
  {
    case C_RE :
        cnt = re_split(string(sp)->str, (sp+2)->ptr) ;
        break ;

    case C_SPACE :
        cnt = space_split(string(sp)->str) ;
        break ;

    /* this case could be done by C_RE, but very slowly.
       Since it is the common way to eliminate fields,
       we'll treat the special case for speed */
    case C_SNULL : /* split on empty string */
        cnt = 1 ;
        temp_buff.ptr_buff[0] = sp->ptr ;
        string(sp)->ref_cnt++ ;
        break ;

    default : bozo("bad splitting cell in bi_split") ;
  }

  /* now load the array */

  free_STRING( string(sp) ) ;

  sp->type = C_DOUBLE ;
  sp->dval = dcnt = (double) cnt ;

  ofmt = string(field + OFMT)->str ;
  A = (ARRAY) (sp+1)->ptr  ;

  while ( cnt )
  { char xbuff[256] ;
    /* this big in case the user did something goofy with
       OFMT  */
  
    (void) sprintf(xbuff, ofmt, dcnt ) ;
    dcnt -= 1.0 ;
    cp = array_find( A, xbuff, 1) ;
    cell_destroy(cp) ;
    cp->ptr = temp_buff.ptr_buff[--cnt] ;
    cp->type = C_MBSTRN ;
  }

  return sp ;
}

