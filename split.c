
/********************************************
split.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	split.c,v $
 * Revision 3.5.1.1  91/09/14  17:24:21  brennan
 * VERSION 1.0
 * 
 * Revision 3.5  91/08/13  06:52:12  brennan
 * VERSION .9994
 * 
 * Revision 3.4  91/07/17  15:11:37  brennan
 * pass length of s to space_split
 * 
 * Revision 3.3  91/06/28  04:17:40  brennan
 * VERSION 0.999
 * 
 * Revision 3.2  91/06/10  15:59:39  brennan
 * changes for V7
 * 
 * Revision 3.1  91/06/07  10:28:23  brennan
 * VERSION 0.995
 * 
 * Revision 2.4  91/06/04  06:48:08  brennan
 * removed <string.h>
 * 
 * Revision 2.3  91/05/28  09:05:18  brennan
 * removed main_buff
 * 
 * Revision 2.2  91/05/15  12:07:41  brennan
 * dval hash table for arrays
 * 
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


/* split string s of length slen on SPACE without changing s.
   load the pieces into STRINGS and ptrs into
   temp_buff.ptr_buff[] 
   return the number of pieces */

int space_split( s , slen)  
  register char *s ;
  unsigned slen ;
{ char *back = s + slen ;
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
    (void) memcpy(sval->str, q, SIZE_T(len)) ;
  }

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
    (void) memcpy(sval->str, s, SIZE_T(len)) ;
    s = t + mlen ;
  }
  temp_buff.ptr_buff[i++] = (PTR) new_STRING(s) ;

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
  CELL c_cnt ; /* CELL version of cnt */
  ARRAY A ;
  CELL  *cp ;


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
        cnt = space_split(string(sp)->str, string(sp)->len) ;
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

  if ( cnt > MAX_SPLIT )
	rt_overflow("maximum pieces from a split", MAX_SPLIT) ;

  /* now load the array */

  free_STRING( string(sp) ) ;

  c_cnt.type = sp->type = C_DOUBLE ;
  c_cnt.dval = sp->dval = (double) cnt ;

  A = (ARRAY) (sp+1)->ptr  ;

  while ( cnt )
  { 
    cp = array_find( A, &c_cnt, CREATE) ;
    cell_destroy(cp) ;
    cp->ptr = temp_buff.ptr_buff[--cnt] ;
    cp->type = C_MBSTRN ;

    c_cnt.dval -= 1.0 ;
  }

  return sp ;
}

