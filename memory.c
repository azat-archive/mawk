
/********************************************
memory.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/* $Log:	memory.c,v $
 * Revision 3.3.1.1  91/09/14  17:23:49  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:51:49  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:17:06  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:27:56  brennan
 * VERSION 0.995
 * 
 * Revision 2.2  91/06/03  07:54:08  brennan
 * changed #ifdef __TURBOC__  to #if HAVE_PROTOS
 * 
 * Revision 2.1  91/04/08  08:23:35  brennan
 * VERSION 0.97
 * 
*/


/* memory.c */

#include "mawk.h"

#if     HAVE_PROTOS
#define SUPPRESS_NEW_STRING_PROTO  /* get compiler off our back on
         the definition of new_STRING() */
#endif

#include "memory.h"

STRING null_str = {1, 0, "" } ;

static STRING *char_string[127] ;
/* slots for strings of one character
   "\01" thru "\177"    */
  
STRING *new_STRING(s, xlen)   
  char *s ;  unsigned xlen ;
  /* WARNING: if s != NULL, don't access xlen
     because it won't be there   */
{ register STRING *p ;
  unsigned len ;

  if ( s )
        switch( len = strlen(s) )
        {
            case 0 : 
                p = &null_str  ; p->ref_cnt++ ;
                break ;

            case 1 :
                if ( *(unsigned char *)s < 128 )
                {   if ( p = char_string[*s-1] )
                        p->ref_cnt++ ;
                    else
                    { p = (STRING *) zmalloc(6) ;
                      p->ref_cnt = 2 ;  p->len = 1 ; 
                      p->str[0] = s[0] ;
                      p->str[1] = 0 ;
                      char_string[*s-1] = p ;
                    }

                    break ; /*case */
                }
                /* else FALL THRU */

            default :
                p = (STRING *) zmalloc(len+5) ;
                p->ref_cnt = 1 ; p->len = len ;
                (void) memcpy( p->str , s, SIZE_T(len+1)) ;
                break ;
        }
  else  
  { p = (STRING *) zmalloc( xlen+5 ) ;
    p->ref_cnt = 1 ; p->len = xlen ;
    /* zero out the end marker */
    p->str[xlen] = 0 ; 
  }

  return p ;
}


#ifdef   DEBUG

void  DB_free_STRING(sval)
  register STRING *sval ;
{ if ( -- sval->ref_cnt == 0 )  zfree(sval, sval->len+5) ; }

#endif
