
/********************************************
memory.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	memory.c,v $
 * Revision 2.1  91/04/08  08:23:35  brennan
 * VERSION 0.97
 * 
*/


/* memory.c */

#include "mawk.h"

#ifdef  __TURBOC__
#define SUPPRESS_NEW_STRING_PROTO  /* get compiler off our back on
         the definition of new_STRING() */
#pragma  warn -pro
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
                (void) memcpy( p->str , s, len+1) ;
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
